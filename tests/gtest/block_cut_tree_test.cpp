#include "decompositions/block_cut_tree.h"

#include "bf_oracles.h"
#include "certificates.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::bf_articulation_vertices;
using graph_recognition::gtest_utils::bf_bridges;
using graph_recognition::gtest_utils::verify_block_cut_tree;

namespace {

std::vector<std::vector<bool>> to_matrix(const Graph& g) {
    std::vector<std::vector<bool>> a(g.n + 1, std::vector<bool>(g.n + 1, false));
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) a[u][g.adj[u][i]] = true;
    }
    return a;
}

std::vector<int> cut_list(const BlockCutTreeResult& r) {
    std::vector<int> cuts;
    for (size_t v = 1; v < r.is_cut.size(); ++v) {
        if (r.is_cut[v]) cuts.push_back((int)v);
    }
    return cuts;
}

TEST(BlockCutTreeTest, EmptyGraph) {
    Graph g;
    BlockCutTreeResult r = compute_block_cut_tree(g);
    EXPECT_TRUE(r.blocks.empty());
    EXPECT_TRUE(r.tree.empty());
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

TEST(BlockCutTreeTest, IsolatedVerticesBecomeK1Blocks) {
    Graph g(3, std::vector<std::pair<int, int>>());
    BlockCutTreeResult r = compute_block_cut_tree(g);
    ASSERT_EQ(r.blocks.size(), 3u);
    for (size_t i = 0; i < r.blocks.size(); ++i) {
        EXPECT_EQ(r.blocks[i].size(), 1u);
        EXPECT_TRUE(r.block_edges[i].empty());
    }
    EXPECT_TRUE(cut_list(r).empty());
    EXPECT_TRUE(r.bridges.empty());
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

TEST(BlockCutTreeTest, PathBlocksAreItsEdges) {
    // P4: three bridge blocks, two cut vertices.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    BlockCutTreeResult r = compute_block_cut_tree(g);
    EXPECT_EQ(r.blocks.size(), 3u);
    EXPECT_EQ(r.bridges.size(), 3u);
    std::vector<int> cuts = cut_list(r);
    ASSERT_EQ(cuts.size(), 2u);
    EXPECT_EQ(cuts[0], 2);
    EXPECT_EQ(cuts[1], 3);
    // Block-cut tree: 3 block nodes + 2 cut nodes, 4 tree edges.
    EXPECT_EQ(r.tree.size(), 5u);
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

TEST(BlockCutTreeTest, CycleIsASingleBlock) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(5, 1));
    Graph g(5, edges);
    BlockCutTreeResult r = compute_block_cut_tree(g);
    ASSERT_EQ(r.blocks.size(), 1u);
    EXPECT_EQ(r.blocks[0].size(), 5u);
    EXPECT_TRUE(cut_list(r).empty());
    EXPECT_TRUE(r.bridges.empty());
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

TEST(BlockCutTreeTest, TwoTrianglesSharingAVertex) {
    // Bowtie: two triangle blocks meeting at the cut vertex 1.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 1));
    edges.push_back(std::make_pair(1, 4));
    edges.push_back(std::make_pair(4, 5));
    edges.push_back(std::make_pair(5, 1));
    Graph g(5, edges);
    BlockCutTreeResult r = compute_block_cut_tree(g);
    ASSERT_EQ(r.blocks.size(), 2u);
    std::vector<int> cuts = cut_list(r);
    ASSERT_EQ(cuts.size(), 1u);
    EXPECT_EQ(cuts[0], 1);
    EXPECT_EQ(r.block_of[1].size(), 2u);
    EXPECT_TRUE(r.bridges.empty());
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

TEST(BlockCutTreeTest, DisconnectedGraphYieldsAForest) {
    // A triangle and a separate edge.
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 1));
    edges.push_back(std::make_pair(4, 5));
    Graph g(5, edges);
    BlockCutTreeResult r = compute_block_cut_tree(g);
    EXPECT_EQ(r.blocks.size(), 2u);
    EXPECT_TRUE(cut_list(r).empty());
    ASSERT_EQ(r.bridges.size(), 1u);
    EXPECT_EQ(r.bridges[0], std::make_pair(4, 5));
    EXPECT_TRUE(verify_block_cut_tree(g, r));
}

// Every graph on up to 6 vertices: the decomposition must satisfy all its
// structural invariants, and cut vertices and bridges must match the
// definition-level oracles.
TEST(BlockCutTreeTest, ExhaustiveAgreementWithBruteForce) {
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
            BlockCutTreeResult r = compute_block_cut_tree(g);

            ASSERT_TRUE(verify_block_cut_tree(g, r)) << "n=" << n << " mask=" << mask;

            std::vector<std::vector<bool>> a = to_matrix(g);
            EXPECT_EQ(cut_list(r), bf_articulation_vertices(n, a))
                << "n=" << n << " mask=" << mask;
            EXPECT_EQ(r.bridges, bf_bridges(n, a)) << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
