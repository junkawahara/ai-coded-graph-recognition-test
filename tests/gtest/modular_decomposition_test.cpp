#include "decompositions/modular_decomposition.h"

#include "bf_oracles.h"
#include "certificates.h"
#include "recognizers/cograph.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::bf_strong_modules;
using graph_recognition::gtest_utils::verify_md_tree;

namespace {

std::vector<std::vector<bool>> to_matrix(const Graph& g) {
    std::vector<std::vector<bool>> a(g.n + 1, std::vector<bool>(g.n + 1, false));
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) a[u][g.adj[u][i]] = true;
    }
    return a;
}

// The vertex sets of the tree nodes, as bitmasks, sorted.
std::vector<int> node_masks(const MDTree& t) {
    std::vector<int> masks;
    for (size_t i = 0; i < t.nodes.size(); ++i) {
        int mask = 0;
        for (size_t j = 0; j < t.nodes[i].vertices.size(); ++j) {
            mask |= 1 << (t.nodes[i].vertices[j] - 1);
        }
        masks.push_back(mask);
    }
    std::sort(masks.begin(), masks.end());
    return masks;
}

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

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

TEST(ModularDecompositionTest, EmptyGraph) {
    Graph g;
    MDTree t = modular_decomposition(g);
    EXPECT_EQ(t.root, -1);
    EXPECT_TRUE(t.nodes.empty());
    EXPECT_TRUE(verify_md_tree(g, t, false));
}

TEST(ModularDecompositionTest, EdgelessGraphIsOneParallelNode) {
    Graph g(3, std::vector<std::pair<int, int>>());
    MDTree t = modular_decomposition(g);
    EXPECT_EQ(canonical(t), "U(1,2,3)");
    EXPECT_TRUE(verify_md_tree(g, t, false));
}

TEST(ModularDecompositionTest, P4IsPrime) {
    // P4 is the smallest prime graph: its only modules are the trivial ones.
    Graph g = make_path(4);
    MDTree t = modular_decomposition(g);
    EXPECT_EQ(canonical(t), "P(1,2,3,4)");
    EXPECT_FALSE(md_is_cotree(t));
    EXPECT_TRUE(verify_md_tree(g, t, false));
}

TEST(ModularDecompositionTest, ModuleInsideAPrimeQuotient) {
    // P4 with vertex 4 doubled by a false twin: the twins form a module that
    // sits inside an otherwise prime quotient.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    edges.push_back(std::make_pair(3, 5));
    Graph g(5, edges);
    MDTree t = modular_decomposition(g);
    EXPECT_EQ(canonical(t), "P(1,2,3,U(4,5))");
    EXPECT_TRUE(verify_md_tree(g, t, false));
}

TEST(ModularDecompositionTest, PathOnThreeVerticesMatchesItsCotree) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    Graph g(3, edges);
    MDTree t = modular_decomposition(g);
    EXPECT_EQ(canonical(t), "J(U(1,3),2)");
    EXPECT_TRUE(md_is_cotree(t));
    EXPECT_EQ(canonical(t), canonical(build_cotree(g).cotree));
}

// Every graph on up to 6 vertices: the tree nodes must be exactly the strong
// modules, the tree must satisfy every structural invariant, and it must have
// no PRIME node exactly when the graph is a cograph -- in which case it must
// equal the cotree the cograph recognizer builds.
TEST(ModularDecompositionTest, ExhaustiveAgreementWithBruteForce) {
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

            MDTree t = modular_decomposition(g);
            ASSERT_TRUE(verify_md_tree(g, t, false)) << "n=" << n << " mask=" << mask;

            ASSERT_EQ(node_masks(t), bf_strong_modules(n, to_matrix(g)))
                << "n=" << n << " mask=" << mask;

            CotreeResult co = build_cotree(g);
            ASSERT_EQ(md_is_cotree(t), co.is_cograph) << "n=" << n << " mask=" << mask;
            if (co.is_cograph) {
                ASSERT_EQ(canonical(t), canonical(co.cotree)) << "n=" << n << " mask=" << mask;
            }
        }
    }
}

}  // namespace
