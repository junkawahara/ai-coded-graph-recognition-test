#include "cochain.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::CochainAlgorithm;
using graph_recognition::CochainResult;
using graph_recognition::check_cochain;

TEST(CochainProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // co-bipartite + chain-like: 2 クリーク間の辺を nested にする
            n = 2 + std::rand() % 8;
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
        CochainResult r1 = check_cochain(g, CochainAlgorithm::COMPLEMENT);
        CochainResult r2 = check_cochain(g, CochainAlgorithm::DIRECT);

        ASSERT_EQ(r1.is_cochain, r2.is_cochain)
            << "COMPLEMENT vs DIRECT trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
