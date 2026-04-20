#include "cochain.h"
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
            // co-bipartite + chain-like: 2 クリーク間の辺を nested にする
            n = 2 + rand() % 8;
            int half = n / 2;
            // 2 クリーク
            for (int u = 1; u <= half; ++u)
                for (int v = u + 1; v <= half; ++v)
                    edges.push_back(std::make_pair(u, v));
            for (int u = half + 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    edges.push_back(std::make_pair(u, v));
            // クリーク間: nested 構造
            for (int u = 1; u <= half; ++u)
                for (int v = half + 1; v <= half + u && v <= n; ++v)
                    edges.push_back(std::make_pair(u, v));
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
        CochainResult r1 = check_cochain(g, CochainAlgorithm::COMPLEMENT);
        CochainResult r2 = check_cochain(g, CochainAlgorithm::DIRECT);

        if (r1.is_cochain != r2.is_cochain) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " COMPLEMENT=" << r1.is_cochain
                      << " DIRECT=" << r2.is_cochain << std::endl;
            // Print edges for debugging
            for (size_t i = 0; i < edges.size(); ++i) {
                std::cerr << "  " << edges[i].first << " " << edges[i].second << std::endl;
            }
            mismatch++;
        }

        total++;
        if (r1.is_cochain) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
