#include "proper_interval.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

int main() {
    srand(42);
    int total = 0, yes_count = 0, no_count = 0, mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // proper interval: path-like structure
            n = 1 + rand() % 9;
            for (int v = 1; v < n; ++v) {
                int range = 1 + rand() % 3;
                for (int u = v + 1; u <= n && u <= v + range; ++u) {
                    edges.push_back(std::make_pair(v, u));
                }
            }
        } else if (trial % 3 == 1) {
            n = 1 + rand() % 9;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + rand() % 9;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        ProperIntervalResult r1 = check_proper_interval(g, ProperIntervalAlgorithm::PQ_TREE);
        ProperIntervalResult r2 = check_proper_interval(g, ProperIntervalAlgorithm::FAST_CLAW_CHECK);

        if (r1.is_proper_interval != r2.is_proper_interval) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " PQ=" << r1.is_proper_interval
                      << " FAST=" << r2.is_proper_interval << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_proper_interval) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
