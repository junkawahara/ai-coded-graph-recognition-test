#include "proper_interval.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ProperIntervalAlgorithm;
using graph_recognition::ProperIntervalResult;
using graph_recognition::check_proper_interval;

TEST(ProperIntervalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // proper interval: path-like structure
            n = 1 + std::rand() % 9;
            for (int v = 1; v < n; ++v) {
                int range = 1 + std::rand() % 3;
                for (int u = v + 1; u <= n && u <= v + range; ++u) {
                    edges.push_back(std::make_pair(v, u));
                }
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
        ProperIntervalResult r1 = check_proper_interval(g, ProperIntervalAlgorithm::PQ_TREE);
        ProperIntervalResult r2 = check_proper_interval(g, ProperIntervalAlgorithm::FAST_CLAW_CHECK);

        ASSERT_EQ(r1.is_proper_interval, r2.is_proper_interval)
            << "PQ vs FAST trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
