#include "series_parallel.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::SeriesParallelAlgorithm;
using graph_recognition::SeriesParallelResult;
using graph_recognition::check_series_parallel;

// Independent brute-force oracle: series-parallel iff K4-minor-free iff
// treewidth <= 2. Treewidth is computed with the classic elimination-ordering
// DP over vertex subsets: f(S) = min over v in S of max(f(S \ {v}),
// q(S \ {v}, v)), where q(S', v) counts the vertices outside S' + {v}
// reachable from v through S'. tw(G) = f(V). This is entirely different
// machinery from the series/parallel reduction used by both library
// implementations, so a shared logic error there cannot hide here.
int bf_treewidth(int n, const std::vector<std::pair<int, int>>& edges) {
    if (n <= 0) return 0;
    std::vector<std::vector<bool>> adj(n, std::vector<bool>(n, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first - 1, v = edges[i].second - 1;
        adj[u][v] = adj[v][u] = true;
    }
    int full = 1 << n;
    std::vector<int> f(full, 0);
    for (int S = 1; S < full; ++S) {
        int best = INT_MAX;
        for (int v = 0; v < n; ++v) {
            if (!((S >> v) & 1)) continue;
            int Sp = S & ~(1 << v);
            // q(Sp, v): vertices outside Sp + {v} reachable from v via Sp
            int q = 0;
            std::vector<bool> vis(n, false);
            std::vector<int> stack;
            stack.push_back(v);
            vis[v] = true;
            while (!stack.empty()) {
                int x = stack.back();
                stack.pop_back();
                for (int w = 0; w < n; ++w) {
                    if (!adj[x][w] || vis[w]) continue;
                    vis[w] = true;
                    if ((Sp >> w) & 1) {
                        stack.push_back(w);
                    } else {
                        ++q;
                    }
                }
            }
            int cand = std::max(f[Sp], q);
            if (cand < best) best = cand;
        }
        f[S] = best;
    }
    return f[full - 1];
}

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

        // 独立オラクル: treewidth <= 2
        bool bf = bf_treewidth(n, edges) <= 2;

        ASSERT_EQ(r1.is_series_parallel, r2.is_series_parallel)
            << "MINOR_CHECK vs QUEUE_REDUCTION trial=" << trial
            << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_series_parallel, bf)
            << "MINOR_CHECK vs treewidth oracle trial=" << trial
            << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
