#include "weakly_chordal.h"
#include "bf_oracles.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::WeaklyChordalAlgorithm;
using graph_recognition::WeaklyChordalResult;
using graph_recognition::check_weakly_chordal;
using graph_recognition::gtest_utils::bf_has_induced_cycle_ge;

// ---- Independent brute-force oracle -------------------------------------
// Weakly chordal <=> neither G nor its complement has an induced cycle of
// length >= 5. The hole check below enumerates vertex subsets directly, so
// it shares nothing with the library's has_induced_cycle_ge5 (which both
// library algorithms rely on and which could therefore hide a common bug).

bool bf_is_weakly_chordal(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    if (bf_has_induced_cycle_ge(n, adj, 5)) return false;
    std::vector<std::vector<bool>> co(n + 1, std::vector<bool>(n + 1, false));
    for (int u = 1; u <= n; ++u)
        for (int v = 1; v <= n; ++v)
            co[u][v] = (u != v && !adj[u][v]);
    return !bf_has_induced_cycle_ge(n, co, 5);
}

TEST(WeaklyChordalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // Biased toward chordal (hence weakly chordal) instances: v
            // attaches to u plus a random subset of N(u), which is NOT
            // necessarily a clique, so chordality is not guaranteed. That is
            // fine: the brute-force oracle decides the ground truth.
            n = 1 + std::rand() % 8;
            std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
                adj[u][v] = adj[v][u] = true;
                for (int w = 1; w < v; ++w) {
                    if (w != u && adj[u][w] && std::rand() % 2 == 0) {
                        edges.push_back(std::make_pair(w, v));
                        adj[w][v] = adj[v][w] = true;
                    }
                }
            }
        } else if (trial % 3 == 1) {
            n = 1 + std::rand() % 8;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + std::rand() % 8;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        WeaklyChordalResult r1 = check_weakly_chordal(g, WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE);
        WeaklyChordalResult r2 = check_weakly_chordal(g, WeaklyChordalAlgorithm::COMPLEMENT_BFS);
        bool bf = bf_is_weakly_chordal(n, edges);

        ASSERT_EQ(r1.is_weakly_chordal, r2.is_weakly_chordal)
            << "CO vs BFS trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_weakly_chordal, bf)
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
