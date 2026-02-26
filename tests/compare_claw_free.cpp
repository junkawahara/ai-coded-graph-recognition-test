#include "claw_free.h"
#include "proper_interval.h"
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

        if (trial % 4 == 0) {
            // claw-free friendly: line-graph-like (path with chords)
            n = 1 + rand() % 9;
            for (int v = 1; v < n; ++v) {
                int range = 1 + rand() % 3;
                for (int u = v + 1; u <= n && u <= v + range; ++u) {
                    edges.push_back(std::make_pair(v, u));
                }
            }
        } else if (trial % 4 == 1) {
            // dense random graph
            n = 1 + rand() % 9;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 4 == 2) {
            // sparse random graph (more likely to have claws)
            n = 1 + rand() % 9;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // star + random edges (likely not claw-free for large n)
            n = 1 + rand() % 8;
            for (int v = 2; v <= n; ++v) {
                edges.push_back(std::make_pair(1, v));
            }
            for (int u = 2; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (rand() % 3 == 0)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        ClawFreeResult r1 = check_claw_free(g, ClawFreeAlgorithm::TRIPLE_LOOP);
        ClawFreeResult r2 = check_claw_free(g, ClawFreeAlgorithm::EDGE_COUNT);

        if (r1.is_claw_free != r2.is_claw_free) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " TRIPLE=" << r1.is_claw_free
                      << " EDGE=" << r2.is_claw_free << std::endl;
            mismatch++;
        }

        // Test 2: proper_interval => claw_free
        ProperIntervalResult pi = check_proper_interval(g);
        if (pi.is_proper_interval && !r1.is_claw_free) {
            std::cerr << "IMPLICATION FAIL at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " proper_interval=YES but claw_free=NO" << std::endl;
            implication_fail++;
        }

        total++;
        if (r1.is_claw_free) yes_count++; else no_count++;
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
