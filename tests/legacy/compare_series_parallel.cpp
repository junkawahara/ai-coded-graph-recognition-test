#include "series_parallel.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

int main() {
    srand(42);

    int total = 0, yes_count = 0, no_count = 0;
    int mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        // ランダムグラフ (疎〜密をバランスよく)
        n = 1 + rand() % 9;
        double p;
        if (trial % 3 == 0) {
            p = (rand() % 30) / 100.0; // 0.0-0.29 (疎: SP になりやすい)
        } else if (trial % 3 == 1) {
            p = (rand() % 90 + 10) / 100.0; // 0.1-0.99
        } else {
            p = (rand() % 50) / 100.0; // 0.0-0.49
        }
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if ((rand() % 1000) / 1000.0 < p) {
                    edges.push_back(std::make_pair(u, v));
                }
            }
        }

        Graph g(n, edges);

        // 両アルゴリズムで判定
        SeriesParallelResult r1 = check_series_parallel(g, SeriesParallelAlgorithm::MINOR_CHECK);
        SeriesParallelResult r2 = check_series_parallel(g, SeriesParallelAlgorithm::QUEUE_REDUCTION);

        if (r1.is_series_parallel != r2.is_series_parallel) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " MINOR_CHECK=" << r1.is_series_parallel
                      << " QUEUE_REDUCTION=" << r2.is_series_parallel << std::endl;
            mismatch++;
        }

        total++;
        if (r1.is_series_parallel) yes_count++;
        else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }

    std::cout << "OK" << std::endl;
    std::cout << "total=" << total
              << " YES=" << yes_count
              << " NO=" << no_count << std::endl;
    return 0;
}
