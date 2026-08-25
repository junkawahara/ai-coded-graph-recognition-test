#include "chordal_bipartite.h"
#include "bf_oracles.h"
#include "graph.h"
#include "certificates.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ChordalBipartiteAlgorithm;
using graph_recognition::ChordalBipartiteResult;
using graph_recognition::check_chordal_bipartite;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::gtest_utils::bf_has_induced_cycle_ge;
using graph_recognition::gtest_utils::bf_is_bipartite;

// ---- Independent brute-force oracle -------------------------------------
// Chordal bipartite <=> bipartite with no induced cycle of length >= 6.
// The hole check enumerates vertex subsets directly, sharing nothing with
// the bisimplicial-elimination or BFS-based cycle search in the library.

bool bf_is_chordal_bipartite(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    return bf_is_bipartite(n, adj) && !bf_has_induced_cycle_ge(n, adj, 6);
}

TEST(ChordalBipartiteProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // tree (always chordal bipartite)
            n = 2 + std::rand() % 8;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
        } else if (trial % 3 == 1) {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        ChordalBipartiteResult r1 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);
        ChordalBipartiteResult r2 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL);
        ChordalBipartiteResult r3 = check_chordal_bipartite(g, ChordalBipartiteAlgorithm::CYCLE_CHECK);
        bool bf = bf_is_chordal_bipartite(n, edges);

        ASSERT_EQ(r1.is_chordal_bipartite, r2.is_chordal_bipartite)
            << "BISIM vs FAST trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r2.is_chordal_bipartite, r3.is_chordal_bipartite)
            << "FAST vs CYCLE trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_chordal_bipartite, bf)
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();

        if (!bf) {
            ASSERT_TRUE(verify_obstruction(g, r1.obstruction))
                << "BISIM witness trial=" << trial << " n=" << n;
            ASSERT_TRUE(verify_obstruction(g, r2.obstruction))
                << "FAST witness trial=" << trial << " n=" << n;
            ASSERT_TRUE(verify_obstruction(g, r3.obstruction))
                << "CYCLE witness trial=" << trial << " n=" << n;
        }
    }
}

}  // namespace
