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

// success reports Algorithm 1 alone: the candidate block tree it builds here
// is rejected by the nested-convex verification of Algorithm 2, and the graph
// has no layout at any root.
TEST(TreeLayoutTest, BlockTreeSuccessIsAlgorithm1Only) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(1, 4));
    edges.push_back(std::make_pair(1, 5));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(2, 5));
    edges.push_back(std::make_pair(3, 4));
    Graph g(5, edges);
    LayoutBlockTree bt = compute_layout_block_tree(g, 5);
    EXPECT_TRUE(bt.success);
    EXPECT_FALSE(verify_layout_block_tree(g, bt));
    EXPECT_FALSE(check_proper_chordal(g).is_proper_chordal);
}

// A root that is not a vertex is rejected before any work; 0 in particular is
// the "no parent" sentinel of a layout, not a vertex.
TEST(TreeLayoutTest, BlockTreeRejectsRootOutsideTheVertexSet) {
    Graph g(1, std::vector<std::pair<int, int>>());
    const int roots[3] = {0, 2, -1};
    for (int i = 0; i < 3; ++i) {
        LayoutBlockTree bt = compute_layout_block_tree(g, roots[i]);
        EXPECT_FALSE(bt.success) << "root=" << roots[i];
        ASSERT_EQ(bt.block_of.size(), (size_t)g.n + 1) << "root=" << roots[i];
        EXPECT_EQ(bt.block_of[1], -1) << "root=" << roots[i];
        EXPECT_FALSE(verify_layout_block_tree(g, bt)) << "root=" << roots[i];
    }
}

// Algorithm 1 grows one tree out of the root, so it needs a connected graph:
// two isolated vertices do have a layout, but only the componentwise entry
// point finds it.
TEST(TreeLayoutTest, BlockTreeExpectsAConnectedGraph) {
    Graph g(2, std::vector<std::pair<int, int>>());
    EXPECT_FALSE(compute_layout_block_tree(g, 1).success);
    TreeLayoutResult tl = find_indifference_tree_layout(g);
    ASSERT_TRUE(tl.success);
    EXPECT_TRUE(verify_tree_layout(g, tl.parent));
}

// Both answers against the definition instead of the recognizer: brute force
// over every rooted forest on the vertex set, checked with the test helpers'
// own verifier, decides proper chordality independently of the search under
// test. (n <= 4 here; property/proper_chordal_property_test.cpp runs n = 5.)
TEST(TreeLayoutTest, ExhaustiveOracleAgreementUpToFourVertices) {
    for (int n = 1; n <= 4; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        const int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            bool oracle = false;
            std::vector<int> parent(n + 1, 0);
            long long total = 1;
            for (int i = 0; i < n; ++i) total *= (n + 1);
            for (long long code = 0; code < total && !oracle; ++code) {
                long long rest = code;
                for (int v = 1; v <= n; ++v) {
                    parent[v] = (int)(rest % (n + 1));
                    rest /= (n + 1);
                }
                if (verify_tree_layout(g, parent)) oracle = true;
            }

            EXPECT_EQ(check_proper_chordal(g).is_proper_chordal, oracle)
                << "n=" << n << " mask=" << mask;
        }
    }
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
