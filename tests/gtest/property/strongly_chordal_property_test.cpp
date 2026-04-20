#include "strongly_chordal.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::StronglyChordalAlgorithm;
using graph_recognition::StronglyChordalResult;
using graph_recognition::check_strongly_chordal;

TEST(StronglyChordalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // strongly chordal: build via simplicial + simple additions
            n = 1 + std::rand() % 9;
            std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
            for (int v = 2; v <= n; ++v) {
                // connect to a random existing clique subset
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
        StronglyChordalResult r1 = check_strongly_chordal(g, StronglyChordalAlgorithm::STRONG_ELIMINATION);
        StronglyChordalResult r2 = check_strongly_chordal(g, StronglyChordalAlgorithm::PEO_MATRIX);
        StronglyChordalResult r3 = check_strongly_chordal(g, StronglyChordalAlgorithm::MCS_SEO);

        ASSERT_EQ(r1.is_strongly_chordal, r2.is_strongly_chordal)
            << "ELIM vs PEO trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_strongly_chordal, r3.is_strongly_chordal)
            << "ELIM vs MCS_SEO trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
