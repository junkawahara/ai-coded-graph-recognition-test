#include "claw_free.h"
#include "proper_interval.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ClawFreeAlgorithm;
using graph_recognition::ClawFreeResult;
using graph_recognition::check_claw_free;
using graph_recognition::ProperIntervalResult;
using graph_recognition::check_proper_interval;

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

        // Test 2: proper_interval => claw_free
        ProperIntervalResult pi = check_proper_interval(g);
        if (pi.is_proper_interval) {
            ASSERT_TRUE(r1.is_claw_free)
                << "proper_interval=>claw_free trial=" << trial
                << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
