#ifndef GRAPH_RECOGNITION_MCS_H
#define GRAPH_RECOGNITION_MCS_H

/**
 * @file mcs.h
 * @brief Maximum Cardinality Search (MCS)
 *
 * 弦グラフに対する完全除去順序 (PEO) を計算するための MCS アルゴリズム。
 */

#include "graph.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief MCS の結果
 */
struct MCSResult {
    std::vector<int> order;   /**< order[i] = 位置 i の頂点 (1-indexed) */
    std::vector<int> number;  /**< number[v] = 頂点 v の位置 (1-indexed) */
};

/**
 * @brief グラフの MCS 順序を計算する
 * @param g 入力グラフ
 * @return MCSResult (order と number)
 *
 * 弦グラフに対しては、結果の order[1..n] が完全除去順序 (PEO) となる。
 * order[1] が最初に除去され、order[n] が最後に除去される。
 */
inline MCSResult mcs(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    std::vector<int> label(n + 1, 0), used(n + 1, 0);
    std::priority_queue<std::pair<int, int>> pq;
    for (int v = 1; v <= n; ++v) pq.push(std::make_pair(0, v));

    for (int i = n; i >= 1; --i) {
        while (!pq.empty()) {
            int v = pq.top().second;
            int l = pq.top().first;
            if (used[v] || l != label[v]) { pq.pop(); continue; }
            pq.pop();
            used[v] = 1;
            res.order[i] = v;
            res.number[v] = i;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (!used[u]) {
                    label[u]++;
                    pq.push(std::make_pair(label[u], u));
                }
            }
            break;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
