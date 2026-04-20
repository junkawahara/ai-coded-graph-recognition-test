#include "chordal_bipartite.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ChordalBipartiteAlgorithm;
using graph_recognition::ChordalBipartiteResult;
using graph_recognition::check_chordal_bipartite;

TEST(ChordalBipartiteProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // bipartite tree (always chordal bipartite)
            n = 2 + std::rand() % 8;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
            // check if bipartite (tree always is, but add extra edges carefully)
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

        ASSERT_EQ(r1.is_chordal_bipartite, r2.is_chordal_bipartite)
            << "BISIM vs FAST trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r2.is_chordal_bipartite, r3.is_chordal_bipartite)
            << "FAST vs CYCLE trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
