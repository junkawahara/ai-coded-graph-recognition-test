#include "line_graph.h"
#include "claw_free.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

int main() {
    srand(42);
    int total = 0, yes_count = 0, no_count = 0;
    int mismatch = 0, implication_fail = 0;

    for (int trial = 0; trial < 5000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 5 == 0) {
            // line graph friendly: L(path) = path
            n = 1 + rand() % 8;
            for (int v = 1; v < n; ++v) {
                edges.push_back(std::make_pair(v, v + 1));
            }
        } else if (trial % 5 == 1) {
            // construct actual line graph: L(H)
            int hn = 2 + rand() % 5;
            std::vector<std::pair<int, int>> hedges;
            for (int u = 1; u <= hn; ++u)
                for (int v = u + 1; v <= hn; ++v)
                    if (rand() % 3 == 0)
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
            n = 1 + rand() % 7;
            double p = (rand() % 80 + 20) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 3) {
            // sparse random graph
            n = 1 + rand() % 7;
            double p = (rand() % 40) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // star + random (likely not line graph)
            n = 1 + rand() % 6;
            for (int v = 2; v <= n; ++v) {
                edges.push_back(std::make_pair(1, v));
            }
            for (int u = 2; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (rand() % 4 == 0)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        LineGraphResult r1 = check_line_graph(g, LineGraphAlgorithm::BRUTE);
        LineGraphResult r2 = check_line_graph(g, LineGraphAlgorithm::KRAUSZ);

        if (r1.is_line_graph != r2.is_line_graph) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " BRUTE=" << r1.is_line_graph
                      << " KRAUSZ=" << r2.is_line_graph << std::endl;
            for (int u = 1; u <= n; ++u)
                for (size_t i = 0; i < g.adj[u].size(); ++i)
                    if (g.adj[u][i] > u)
                        std::cerr << " " << u << "-" << g.adj[u][i];
            std::cerr << std::endl;
            mismatch++;
        }

        // Test 2: line_graph => claw_free
        ClawFreeResult cf = check_claw_free(g);
        if (r1.is_line_graph && !cf.is_claw_free) {
            std::cerr << "IMPLICATION FAIL at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " line_graph=YES but claw_free=NO" << std::endl;
            implication_fail++;
        }

        total++;
        if (r1.is_line_graph) yes_count++; else no_count++;
    }

    if (mismatch > 0 || implication_fail > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches, "
                  << implication_fail << " implication failures" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
