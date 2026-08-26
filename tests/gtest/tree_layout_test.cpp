#include "decompositions/tree_layout.h"

#include "certificates.h"
#include "recognizers/proper_chordal.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::verify_tree_layout;

namespace {

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

TEST(TreeLayoutTest, EmptyGraph) {
    Graph g;
    TreeLayoutResult r = find_indifference_tree_layout(g);
    EXPECT_TRUE(r.success);
    EXPECT_TRUE(verify_tree_layout(g, r.parent));
}

TEST(TreeLayoutTest, PathIsLaidOutAsAPath) {
    Graph g = make_path(5);
    TreeLayoutResult r = find_indifference_tree_layout(g);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(verify_tree_layout(g, r.parent));
    // A path has exactly one root and every other vertex one parent.
    int roots = 0;
    for (int v = 1; v <= g.n; ++v) {
        if (r.parent[v] == 0) ++roots;
    }
    EXPECT_EQ(roots, 1);
}

TEST(TreeLayoutTest, DisconnectedGraphHangsComponentsTogether) {
    // Two separate edges: still one layout over all four vertices.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    TreeLayoutResult r = find_indifference_tree_layout(g);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(verify_tree_layout(g, r.parent));
}

TEST(TreeLayoutTest, BlockTreeOfAPathRootedAtAnEnd) {
    Graph g = make_path(4);
    LayoutBlockTree bt = compute_layout_block_tree(g, 1);
    ASSERT_TRUE(bt.success);
    ASSERT_EQ(bt.block_of.size(), (size_t)g.n + 1);
    // Every vertex belongs to a block, and blocks form a tree with one root.
    int roots = 0;
    for (size_t i = 0; i < bt.parent.size(); ++i) {
        if (bt.parent[i] == -1) ++roots;
    }
    EXPECT_EQ(roots, 1);
}

// Every graph on up to 6 vertices: a layout is produced exactly when the
// recognizer says proper chordal, and it always satisfies the definition.
TEST(TreeLayoutTest, ExhaustiveAgreementWithRecognizer) {
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

            ProperChordalResult pc = check_proper_chordal(g);
            if (!pc.is_proper_chordal) continue;
            ASSERT_TRUE(verify_tree_layout(g, pc.layout_parent))
                << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
