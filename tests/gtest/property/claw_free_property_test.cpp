#include "recognizers/claw_free.h"
#include "bf_oracles.h"
#include "certificates.h"
#include "recognizers/proper_interval.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ClawFreeAlgorithm;
using graph_recognition::ClawFreeResult;
using graph_recognition::check_claw_free;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::ProperIntervalResult;
using graph_recognition::check_proper_interval;

// Independent brute-force oracle straight from the definition: an induced
// claw is a center adjacent to three pairwise non-adjacent vertices.
bool bf_is_claw_free(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    return !graph_recognition::gtest_utils::bf_has_claw(n, adj);
}

TEST(ClawFreeProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 5000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 4 == 0) {
            // claw-free friendly: line-graph-like (path with chords)
            n = 1 + std::rand() % 9;
            for (int v = 1; v < n; ++v) {
                int range = 1 + std::rand() % 3;
                for (int u = v + 1; u <= n && u <= v + range; ++u) {
                    edges.push_back(std::make_pair(v, u));
                }
            }
        } else if (trial % 4 == 1) {
            // dense random graph
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 4 == 2) {
            // sparse random graph (more likely to have claws)
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // star + random edges (likely not claw-free for large n)
            n = 1 + std::rand() % 8;
            for (int v = 2; v <= n; ++v) {
                edges.push_back(std::make_pair(1, v));
            }
            for (int u = 2; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (std::rand() % 3 == 0)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        ClawFreeResult r1 = check_claw_free(g, ClawFreeAlgorithm::TRIPLE_LOOP);
        ClawFreeResult r2 = check_claw_free(g, ClawFreeAlgorithm::EDGE_COUNT);

        ASSERT_EQ(r1.is_claw_free, r2.is_claw_free)
            << "TRIPLE vs EDGE trial=" << trial << " n=" << n << " m=" << edges.size();

        // Test 2: independent brute-force oracle
        ASSERT_EQ(r1.is_claw_free, bf_is_claw_free(n, edges))
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();

        if (!r1.is_claw_free) {
            ASSERT_TRUE(verify_obstruction(g, r1.obstruction))
                << "TRIPLE_LOOP trial=" << trial << " n=" << n;
            ASSERT_TRUE(verify_obstruction(g, r2.obstruction))
                << "EDGE_COUNT trial=" << trial << " n=" << n;
        }

        // Test 3: proper_interval => claw_free
        ProperIntervalResult pi = check_proper_interval(g);
        if (pi.is_proper_interval) {
            ASSERT_TRUE(r1.is_claw_free)
                << "proper_interval=>claw_free trial=" << trial
                << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
