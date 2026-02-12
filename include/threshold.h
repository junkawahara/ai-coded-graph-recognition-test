#ifndef GRAPH_RECOGNITION_THRESHOLD_H
#define GRAPH_RECOGNITION_THRESHOLD_H

/**
 * @file threshold.h
 * @brief 閾値グラフ (threshold graph) 認識
 *
 * アルゴリズム:
 *   - DEGREE_SEQUENCE: 孤立/全域頂点の反復除去
 *   - DEGREE_SEQUENCE_FAST: 次数列ソート + 両端ポインタ (デフォルト)
 */

#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief 閾値グラフ認識アルゴリズムの選択
 */
enum class ThresholdAlgorithm {
    DEGREE_SEQUENCE,      /**< 反復除去 */
    DEGREE_SEQUENCE_FAST  /**< 次数列ソート + 両端ポインタ (デフォルト) */
};

/**
 * @brief 閾値グラフ認識の結果
 */
struct ThresholdResult {
    bool is_threshold; /**< 閾値グラフであれば true */
};

namespace detail {

/**
 * @brief 反復除去による閾値グラフ認識 (元のアルゴリズム)
 */
inline ThresholdResult check_threshold_elimination(const Graph& g) {
    ThresholdResult res;
    res.is_threshold = true;

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

    int alive_count = n;
    for (int step = 0; step < n; ++step) {
        int pick = 0;
        for (int v = 1; v <= n; ++v) {
            if (!alive[v]) continue;
            if (degree[v] == 0 || degree[v] == alive_count - 1) {
                pick = v;
                break;
            }
        }
        if (pick == 0) {
            res.is_threshold = false;
            return res;
        }

        alive[pick] = 0;
        alive_count--;
        for (size_t i = 0; i < neighbors[pick].size(); ++i) {
            int u = neighbors[pick][i];
            if (alive[u]) degree[u]--;
        }
    }

    return res;
}

/**
 * @brief 次数列ソート + 両端ポインタによる閾値グラフ認識
 *
 * 閾値グラフは次数列で一意に決まる (unigraph)。
 * 次数列を降順ソートし、両端から孤立/全域頂点を
 * lazy offset で O(n) シミュレーションする。
 */
inline ThresholdResult check_threshold_fast(const Graph& g) {
    ThresholdResult res;
    res.is_threshold = true;

    int n = g.n;
    if (n <= 1) return res;

    // 次数計算
    std::vector<int> deg(n);
    for (int v = 1; v <= n; ++v) {
        deg[v - 1] = (int)g.adj[v].size();
    }

    // カウンティングソート (降順)
    std::vector<int> count(n, 0);
    for (int i = 0; i < n; ++i) count[deg[i]]++;
    std::vector<int> d(n);
    // 降順: 高い次数から配置
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        for (int c = 0; c < count[k]; ++c) {
            d[pos++] = k;
        }
    }

    // 両端ポインタ + lazy offset
    int lo = 0, hi = n - 1;
    int remaining = n;
    int offset = 0;

    while (lo <= hi) {
        int actual_hi = d[hi] - offset;
        int actual_lo = d[lo] - offset;

        if (actual_hi == 0) {
            // 孤立頂点を除去
            hi--;
            remaining--;
        } else if (actual_lo == remaining - 1) {
            // 全域頂点を除去
            lo++;
            remaining--;
            offset++;
        } else {
            res.is_threshold = false;
            return res;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが閾値グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DEGREE_SEQUENCE_FAST)
 * @return ThresholdResult
 */
inline ThresholdResult check_threshold(const Graph& g,
    ThresholdAlgorithm algo = ThresholdAlgorithm::DEGREE_SEQUENCE_FAST) {
    switch (algo) {
        case ThresholdAlgorithm::DEGREE_SEQUENCE:
            return detail::check_threshold_elimination(g);
        case ThresholdAlgorithm::DEGREE_SEQUENCE_FAST:
            return detail::check_threshold_fast(g);
    }
    return ThresholdResult();
}

} // namespace graph_recognition

#endif
