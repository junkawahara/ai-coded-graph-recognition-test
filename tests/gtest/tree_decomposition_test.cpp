#include "decompositions/tree_decomposition.h"

#include "bf_oracles.h"
#include "certificates.h"
#include "recognizers/ktree.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::bf_treewidth;
using graph_recognition::gtest_utils::verify_tree_decomposition;

namespace {

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

Graph make_complete(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(n, edges);
}

TEST(TreeDecompositionTest, EmptyGraph) {
    Graph g;
    TreeDecompositionResult r = tree_decomposition_chordal(g);
    EXPECT_TRUE(r.success);
    EXPECT_EQ(r.width, -1);
    EXPECT_TRUE(verify_tree_decomposition(g, r));
}

TEST(TreeDecompositionTest, PathHasWidthOne) {
    Graph g = make_path(5);
    TreeDecompositionResult r = tree_decomposition_chordal(g);
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.width, 1);
    EXPECT_EQ(r.bags.size(), 4u);  // one bag per edge
    EXPECT_TRUE(verify_tree_decomposition(g, r));
}

TEST(TreeDecompositionTest, CompleteGraphIsOneBag) {
    Graph g = make_complete(4);
    TreeDecompositionResult r = tree_decomposition_chordal(g);
    ASSERT_TRUE(r.success);
    ASSERT_EQ(r.bags.size(), 1u);
    EXPECT_EQ(r.width, 3);
    EXPECT_TRUE(verify_tree_decomposition(g, r));
}

TEST(TreeDecompositionTest, EdgelessGraphHasWidthZero) {
    Graph g(3, std::vector<std::pair<int, int>>());
    TreeDecompositionResult r = tree_decomposition_chordal(g);
    ASSERT_TRUE(r.success);
    EXPECT_EQ(r.width, 0);
    EXPECT_TRUE(verify_tree_decomposition(g, r));
}

TEST(TreeDecompositionTest, NonChordalGraphIsRejected) {
    // C4 is not chordal, so no clique tree and no decomposition here.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    edges.push_back(std::make_pair(4, 1));
    Graph g(4, edges);
    TreeDecompositionResult r = tree_decomposition_chordal(g);
    EXPECT_FALSE(r.success);
    EXPECT_EQ(r.width, -1);
}

TEST(TreeDecompositionTest, ViolatedCliqueTreePreconditionIsRejected) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    edges.push_back(std::make_pair(4, 1));
    Graph g(4, edges);
    ChordalResult ch = check_chordal(g);
    ASSERT_FALSE(ch.is_chordal);
    // build_clique_tree on a non-chordal graph returns an empty result; the
    // decomposition must not read that as "a graph with no cliques".
    TreeDecompositionResult r =
        tree_decomposition_from_clique_tree(g, build_clique_tree(g, ch));
    EXPECT_FALSE(r.success);
}

// Every graph on up to 6 vertices: for the chordal ones the clique tree must
// be a valid tree decomposition, and its width must equal the treewidth
// computed by an independent subset DP. A k-tree's width must be exactly k.
TEST(TreeDecompositionTest, ExhaustiveWidthMatchesTreewidthOracle) {
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

            TreeDecompositionResult r = tree_decomposition_chordal(g);
            ASSERT_EQ(r.success, check_chordal(g).is_chordal)
                << "n=" << n << " mask=" << mask;
            if (!r.success) continue;

            ASSERT_TRUE(verify_tree_decomposition(g, r)) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(r.width, bf_treewidth(n, edges)) << "n=" << n << " mask=" << mask;

            KTreeResult kt = check_ktree(g);
            if (kt.is_ktree) {
                ASSERT_EQ(r.width, kt.k) << "n=" << n << " mask=" << mask;
            }
        }
    }
}

}  // namespace
