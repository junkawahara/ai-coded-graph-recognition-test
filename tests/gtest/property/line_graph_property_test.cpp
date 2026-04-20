#include "line_graph.h"
#include "claw_free.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::LineGraphAlgorithm;
using graph_recognition::LineGraphResult;
using graph_recognition::check_line_graph;
using graph_recognition::ClawFreeResult;
using graph_recognition::check_claw_free;

TEST(LineGraphProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 5000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 5 == 0) {
            // line graph friendly: L(path) = path
            n = 1 + std::rand() % 8;
            for (int v = 1; v < n; ++v) {
                edges.push_back(std::make_pair(v, v + 1));
            }
        } else if (trial % 5 == 1) {
            // construct actual line graph: L(H)
            int hn = 2 + std::rand() % 5;
            std::vector<std::pair<int, int>> hedges;
            for (int u = 1; u <= hn; ++u)
                for (int v = u + 1; v <= hn; ++v)
                    if (std::rand() % 3 == 0)
                        hedges.push_back(std::make_pair(u, v));
            n = (int)hedges.size();
            if (n == 0) { n = 1; }
            else {
                for (int i = 0; i < (int)hedges.size(); ++i) {
                    for (int j = i + 1; j < (int)hedges.size(); ++j) {
                        if (hedges[i].first == hedges[j].first ||
                            hedges[i].first == hedges[j].second ||
                            hedges[i].second == hedges[j].first ||
                            hedges[i].second == hedges[j].second) {
                            edges.push_back(std::make_pair(i + 1, j + 1));
                        }
                    }
                }
            }
        } else if (trial % 5 == 2) {
            // dense random graph
            n = 1 + std::rand() % 7;
            double p = (std::rand() % 80 + 20) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 3) {
            // sparse random graph
            n = 1 + std::rand() % 7;
            double p = (std::rand() % 40) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // star + random (likely not line graph)
            n = 1 + std::rand() % 6;
            for (int v = 2; v <= n; ++v) {
                edges.push_back(std::make_pair(1, v));
            }
            for (int u = 2; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (std::rand() % 4 == 0)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        LineGraphResult r1 = check_line_graph(g, LineGraphAlgorithm::BRUTE);
        LineGraphResult r2 = check_line_graph(g, LineGraphAlgorithm::KRAUSZ);

        ASSERT_EQ(r1.is_line_graph, r2.is_line_graph)
            << "BRUTE vs KRAUSZ trial=" << trial << " n=" << n << " m=" << edges.size();

        // Test 2: line_graph => claw_free
        if (r1.is_line_graph) {
            ClawFreeResult cf = check_claw_free(g);
            ASSERT_TRUE(cf.is_claw_free)
                << "line_graph=>claw_free trial=" << trial << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
