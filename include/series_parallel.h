#ifndef GRAPH_RECOGNITION_SERIES_PARALLEL_H
#define GRAPH_RECOGNITION_SERIES_PARALLEL_H

/**
 * @file series_parallel.h
 * @brief 直並列グラフ (series-parallel graph) 認識
 *
 * アルゴリズム:
 *   - MINOR_CHECK: 次数 2 以下の頂点を全スキャンで反復除去
 *   - QUEUE_REDUCTION: キューベースの 2-退化性テスト (デフォルト)
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 直並列グラフ認識アルゴリズムの選択
 */
enum class SeriesParallelAlgorithm {
    MINOR_CHECK,    /**< 全スキャンによる反復除去 */
    QUEUE_REDUCTION /**< キューベースの 2-退化性テスト (デフォルト) */
};

/**
 * @brief 直並列グラフ認識の結果
 */
struct SeriesParallelResult {
    bool is_series_parallel; /**< 直並列グラフであれば true */
};

namespace detail {

/** @brief 全スキャンによる反復除去 (元のアルゴリズム) */
inline SeriesParallelResult check_series_parallel_scan(const Graph& g) {
    SeriesParallelResult res;
    res.is_series_parallel = true;

    int n = g.n;
    std::vector<std::vector<int>> neighbors(n + 1);
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        neighbors[v].reserve(g.adj_set[v].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[v].begin();
             it != g.adj_set[v].end(); ++it) {
            neighbors[v].push_back(*it);
        }
        degree[v] = (int)neighbors[v].size();
    }

    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] <= 2) {
                pick = v;
                break;
            }
        }

        if (pick == 0) {
            res.is_series_parallel = false;
            return res;
        }

        alive[pick] = 0;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief キューベースの 2-退化性テスト
 *
 * 次数 ≤ 2 の頂点をキューで管理し、除去時に隣接頂点の次数を更新。
 * 全頂点を除去できれば 2-退化 (= K4-minor-free = 直並列)。
 */
inline SeriesParallelResult check_series_parallel_queue(const Graph& g) {
    SeriesParallelResult res;
    res.is_series_parallel = true;

    int n = g.n;
    std::vector<int> degree(n + 1, 0);
    std::vector<unsigned char> alive(n + 1, 1);

    for (int v = 1; v <= n; ++v) {
        degree[v] = (int)g.adj[v].size();
    }

    std::queue<int> q;
    for (int v = 1; v <= n; ++v) {
        if (degree[v] <= 2) q.push(v);
    }

    int removed = 0;
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (!alive[v]) continue;
        if (degree[v] > 2) continue;

        alive[v] = 0;
        removed++;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (alive[u]) {
                degree[u]--;
                if (degree[u] <= 2) q.push(u);
            }
        }
    }

    if (removed != n) {
        res.is_series_parallel = false;
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが直並列グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: QUEUE_REDUCTION)
 * @return SeriesParallelResult
 */
inline SeriesParallelResult check_series_parallel(const Graph& g,
    SeriesParallelAlgorithm algo = SeriesParallelAlgorithm::QUEUE_REDUCTION) {
    switch (algo) {
        case SeriesParallelAlgorithm::MINOR_CHECK:
            return detail::check_series_parallel_scan(g);
        case SeriesParallelAlgorithm::QUEUE_REDUCTION:
            return detail::check_series_parallel_queue(g);
    }
    return SeriesParallelResult();
}

} // namespace graph_recognition

#endif
