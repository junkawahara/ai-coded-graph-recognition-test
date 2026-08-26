#include "recognizers/cograph.h"
#include "decompositions/md_tree.h"

#include "certificates.h"
#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::verify_md_tree;

namespace {

Graph make_p4() {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    return Graph(4, edges);
}

// Canonical serialization, so trees from the two algorithms can be compared.
void serialize(const MDTree& t, int node, std::string& out) {
    const MDNode& n = t.nodes[node];
    if (n.kind == MDNodeKind::LEAF) {
        out += std::to_string(n.vertex);
        return;
    }
    out += (n.kind == MDNodeKind::SERIES ? "J(" : (n.kind == MDNodeKind::PARALLEL ? "U(" : "P("));
    for (size_t i = 0; i < n.children.size(); ++i) {
        if (i) out += ",";
        serialize(t, n.children[i], out);
    }
    out += ")";
}

std::string canonical(const MDTree& t) {
    if (t.root < 0) return "";
    std::string out;
    serialize(t, t.root, out);
    return out;
}

TEST(CotreeTest, EmptyGraphHasNoTree) {
    Graph g;
    CotreeResult r = build_cotree(g);
    EXPECT_TRUE(r.is_cograph);
    EXPECT_EQ(r.cotree.root, -1);
    EXPECT_TRUE(r.cotree.nodes.empty());
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

TEST(CotreeTest, SingleVertexIsALeaf) {
    Graph g(1, std::vector<std::pair<int, int>>());
    CotreeResult r = build_cotree(g);
    ASSERT_TRUE(r.is_cograph);
    ASSERT_EQ(r.cotree.nodes.size(), 1u);
    EXPECT_EQ(r.cotree.nodes[r.cotree.root].kind, MDNodeKind::LEAF);
    EXPECT_EQ(canonical(r.cotree), "1");
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

TEST(CotreeTest, EdgelessGraphIsOneParallelNode) {
    Graph g(3, std::vector<std::pair<int, int>>());
    CotreeResult r = build_cotree(g);
    ASSERT_TRUE(r.is_cograph);
    EXPECT_EQ(canonical(r.cotree), "U(1,2,3)");
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

TEST(CotreeTest, CompleteGraphIsOneSeriesNode) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 3; ++u)
        for (int v = u + 1; v <= 3; ++v) edges.push_back(std::make_pair(u, v));
    Graph g(3, edges);
    CotreeResult r = build_cotree(g);
    ASSERT_TRUE(r.is_cograph);
    EXPECT_EQ(canonical(r.cotree), "J(1,2,3)");
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

TEST(CotreeTest, PathOnThreeVerticesJoinsACenterToTwoLeaves) {
    // P3 = 1-2-3 is the join of {2} with the union of {1} and {3}.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    Graph g(3, edges);
    CotreeResult r = build_cotree(g);
    ASSERT_TRUE(r.is_cograph);
    EXPECT_EQ(canonical(r.cotree), "J(U(1,3),2)");
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

TEST(CotreeTest, P4HasNoCotree) {
    Graph g = make_p4();
    CotreeResult r = build_cotree(g);
    EXPECT_FALSE(r.is_cograph);
    EXPECT_TRUE(r.cotree.nodes.empty());
    EXPECT_EQ(r.cotree.root, -1);
}

TEST(CotreeTest, DisconnectedCographKeepsBothParts) {
    // K2 and K2: a union of two joins.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    CotreeResult r = build_cotree(g);
    ASSERT_TRUE(r.is_cograph);
    EXPECT_EQ(canonical(r.cotree), "U(J(1,2),J(3,4))");
    EXPECT_TRUE(verify_md_tree(g, r.cotree, true));
}

// Every graph on up to 6 vertices: the tree must be a valid cotree exactly
// when the recognizers say cograph, and both algorithms must produce the same
// canonical tree.
TEST(CotreeTest, ExhaustiveAgreementWithRecognizers) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            bool expect = check_cograph(g).is_cograph;
            ASSERT_EQ(check_cograph(g, CographAlgorithm::COTREE).is_cograph, expect)
                << "n=" << n << " mask=" << mask;

            CotreeResult slow = build_cotree(g, CographAlgorithm::COTREE);
            CotreeResult fast = build_cotree(g, CographAlgorithm::PARTITION_REFINEMENT);
            CotreeResult modular = build_cotree(g, CographAlgorithm::MODULAR);
            ASSERT_EQ(slow.is_cograph, expect) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(fast.is_cograph, expect) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(modular.is_cograph, expect) << "n=" << n << " mask=" << mask;

            if (!expect) continue;
            ASSERT_TRUE(verify_md_tree(g, slow.cotree, true)) << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(verify_md_tree(g, fast.cotree, true)) << "n=" << n << " mask=" << mask;
            // The components and co-components at each step are determined by
            // the graph, so the two searches must agree once the children are
            // in canonical order.
            ASSERT_EQ(canonical(slow.cotree), canonical(fast.cotree))
                << "n=" << n << " mask=" << mask;
            // The modular decomposition arrives at the same tree by entirely
            // different machinery.
            ASSERT_TRUE(verify_md_tree(g, modular.cotree, true)) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(canonical(slow.cotree), canonical(modular.cotree))
                << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
