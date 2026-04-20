#include "series_parallel.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::SeriesParallelAlgorithm;
using graph_recognition::SeriesParallelResult;
using graph_recognition::check_series_parallel;

TEST(SeriesParallelProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        // ランダムグラフ (疎〜密をバランスよく)
        n = 1 + std::rand() % 9;
        double p;
        if (trial % 3 == 0) {
            p = (std::rand() % 30) / 100.0; // 0.0-0.29 (疎: SP になりやすい)
        } else if (trial % 3 == 1) {
            p = (std::rand() % 90 + 10) / 100.0; // 0.1-0.99
        } else {
            p = (std::rand() % 50) / 100.0; // 0.0-0.49
        }
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if ((std::rand() % 1000) / 1000.0 < p) {
                    edges.push_back(std::make_pair(u, v));
                }
            }
        }

        Graph g(n, edges);

        // 両アルゴリズムで判定
        SeriesParallelResult r1 = check_series_parallel(g, SeriesParallelAlgorithm::MINOR_CHECK);
        SeriesParallelResult r2 = check_series_parallel(g, SeriesParallelAlgorithm::QUEUE_REDUCTION);

        ASSERT_EQ(r1.is_series_parallel, r2.is_series_parallel)
            << "MINOR_CHECK vs QUEUE_REDUCTION trial=" << trial
            << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
