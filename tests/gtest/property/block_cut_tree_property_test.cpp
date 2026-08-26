#include "recognizers/biconnected.h"
#include "decompositions/block_cut_tree.h"
#include "recognizers/cactus.h"
#include "util/graph.h"

#include "bf_oracles.h"
#include "certificates.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::BiconnectedAlgorithm;
using graph_recognition::BlockCutTreeResult;
using graph_recognition::CactusAlgorithm;
using graph_recognition::Graph;
using graph_recognition::check_biconnected;
using graph_recognition::check_cactus;
using graph_recognition::compute_block_cut_tree;
using graph_recognition::gtest_utils::bf_articulation_vertices;
using graph_recognition::gtest_utils::bf_bridges;
using graph_recognition::gtest_utils::verify_block_cut_tree;

std::vector<std::vector<bool>> to_matrix(int n, const Graph& g) {
    std::vector<std::vector<bool>> a(n + 1, std::vector<bool>(n + 1, false));
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) a[u][g.adj[u][i]] = true;
    }
    return a;
}

// A random cactus: grow by attaching either a pendant edge or a cycle to an
// existing vertex, so every block is an edge or a cycle by construction.
std::vector<std::pair<int, int>> gen_random_cactus(int n) {
    std::vector<std::pair<int, int>> edges;
    int used = 1;
    while (used < n) {
        int anchor = 1 + std::rand() % used;
        int remaining = n - used;
        if (std::rand() % 2 == 0 || remaining < 2) {
            edges.push_back(std::make_pair(anchor, used + 1));
            used += 1;
        } else {
            int len = 2 + std::rand() % (remaining < 4 ? remaining : 4);
            int prev = anchor;
            for (int i = 0; i < len; ++i) {
                edges.push_back(std::make_pair(prev, used + i + 1));
                prev = used + i + 1;
            }
            edges.push_back(std::make_pair(prev, anchor));
            used += len;
        }
    }
    return edges;
}

TEST(BlockCutTreeProperty, RandomTrialsMatchOraclesAndVariants) {
    std::srand(1234);

    for (int trial = 0; trial < 2000; ++trial) {
        int n = 1 + std::rand() % 9;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            edges = gen_random_cactus(n);
        } else if (trial % 3 == 1) {
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        BlockCutTreeResult bct = compute_block_cut_tree(g);
        ASSERT_TRUE(verify_block_cut_tree(g, bct))
            << "trial=" << trial << " n=" << n << " m=" << edges.size();

        std::vector<std::vector<bool>> a = to_matrix(n, g);
        std::vector<int> cuts;
        for (int v = 1; v <= n; ++v)
            if (bct.is_cut[v]) cuts.push_back(v);
        ASSERT_EQ(cuts, bf_articulation_vertices(n, a)) << "trial=" << trial;
        ASSERT_EQ(bct.bridges, bf_bridges(n, a)) << "trial=" << trial;

        ASSERT_EQ(check_biconnected(g, BiconnectedAlgorithm::DFS).is_biconnected,
                  check_biconnected(g, BiconnectedAlgorithm::BLOCK_CUT_TREE).is_biconnected)
            << "biconnected trial=" << trial << " n=" << n;
        ASSERT_EQ(check_cactus(g, CactusAlgorithm::DFS).is_cactus,
                  check_cactus(g, CactusAlgorithm::BLOCK_CUT_TREE).is_cactus)
            << "cactus trial=" << trial << " n=" << n;
    }
}

}  // namespace
