#ifndef GRAPH_RECOGNITION_MCS_H
#define GRAPH_RECOGNITION_MCS_H

/**
 * @file mcs.h
 * @brief Maximum Cardinality Search (MCS)
 *
 * アルゴリズム:
 *   - PQ_MCS: 優先度キューベースの MCS O(n+m log n)
 *   - BUCKET_MCS: バケットソートベースの MCS O(n+m) (デフォルト)
 */

#include "graph.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief MCS アルゴリズムの選択
 */
enum class MCSAlgorithm {
    PQ_MCS,     /**< 優先度キューベース O(n+m log n) */
    BUCKET_MCS  /**< バケットソートベース O(n+m) (デフォルト) */
};

/**
 * @brief MCS の結果
 */
struct MCSResult {
    std::vector<int> order;   /**< order[i] = 位置 i の頂点 (1-indexed) */
    std::vector<int> number;  /**< number[v] = 頂点 v の位置 (1-indexed) */
};

namespace detail {

/**
 * @brief 優先度キューベースの MCS (元のアルゴリズム)
 */
inline MCSResult mcs_pq(const Graph& g) {
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

/**
 * @brief バケットソートベースの MCS O(n+m)
 *
 * バケット配列で各ラベル値の頂点を双方向リストで管理する。
 * ラベル更新はバケット間の移動で O(1)。
 */
inline MCSResult mcs_bucket(const Graph& g) {
    int n = g.n;
    MCSResult res;
    res.order.resize(n + 1, 0);
    res.number.resize(n + 1, 0);

    if (n == 0) return res;

    std::vector<int> key(n + 1, 0);        // key[v] = ラベル済み隣接数
    std::vector<unsigned char> used(n + 1, 0);

    // 双方向リスト: prev[v], next[v]
    // bucket_head[k] = ラベル k のリストの先頭 (0 なら空)
    std::vector<int> prev(n + 1, 0), next(n + 1, 0);
    std::vector<int> bucket_head(n + 1, 0); // bucket_head[0..n-1]

    // 全頂点をバケット 0 に挿入
    bucket_head[0] = 1;
    prev[1] = 0;
    for (int v = 1; v <= n; ++v) {
        next[v] = (v < n) ? v + 1 : 0;
        prev[v] = (v > 1) ? v - 1 : 0;
    }

    int max_key = 0;

    for (int i = n; i >= 1; --i) {
        // max_key のバケットから頂点を取り出す
        while (max_key >= 0 && bucket_head[max_key] == 0) max_key--;
        int v = bucket_head[max_key];

        // リストから v を除去
        bucket_head[max_key] = next[v];
        if (next[v] != 0) prev[next[v]] = 0;

        used[v] = 1;
        res.order[i] = v;
        res.number[v] = i;

        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (used[u]) continue;

            int old_key = key[u];

            // バケット old_key から u を除去
            if (prev[u] != 0) {
                next[prev[u]] = next[u];
            } else {
                bucket_head[old_key] = next[u];
            }
            if (next[u] != 0) {
                prev[next[u]] = prev[u];
            }

            key[u] = old_key + 1;

            // バケット old_key+1 に u を挿入 (先頭)
            next[u] = bucket_head[key[u]];
            prev[u] = 0;
            if (bucket_head[key[u]] != 0) {
                prev[bucket_head[key[u]]] = u;
            }
            bucket_head[key[u]] = u;

            if (key[u] > max_key) max_key = key[u];
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフの MCS 順序を計算する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: BUCKET_MCS)
 * @return MCSResult (order と number)
 *
 * 弦グラフに対しては、結果の order[1..n] が完全除去順序 (PEO) となる。
 * order[1] が最初に除去され、order[n] が最後に除去される。
 */
inline MCSResult mcs(const Graph& g,
    MCSAlgorithm algo = MCSAlgorithm::BUCKET_MCS) {
    switch (algo) {
        case MCSAlgorithm::PQ_MCS:
            return detail::mcs_pq(g);
        case MCSAlgorithm::BUCKET_MCS:
            return detail::mcs_bucket(g);
    }
    return MCSResult();
}

} // namespace graph_recognition

#endif
