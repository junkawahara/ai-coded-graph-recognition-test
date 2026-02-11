#ifndef GRAPH_RECOGNITION_THRESHOLD_H
#define GRAPH_RECOGNITION_THRESHOLD_H

/**
 * @file threshold.h
 * @brief 閾値グラフ (threshold graph) 認識
 *
 * 孤立頂点または全域頂点の繰り返し除去により閾値グラフを認識する。
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 閾値グラフ認識アルゴリズムの選択
 */
enum class ThresholdAlgorithm {
    DEGREE_SEQUENCE /**< 次数列による判定 */
};

/**
 * @brief 閾値グラフ認識の結果
 */
struct ThresholdResult {
    bool is_threshold; /**< 閾値グラフであれば true */
};

/**
 * @brief グラフが閾値グラフか判定する
 * @param g 入力グラフ
 * @return ThresholdResult
 *
 * 孤立頂点 (次数 0) または全域頂点 (次数 = 残り頂点数 - 1) を
 * 繰り返し除去し、全頂点を削除できれば閾値グラフ。
 */
inline ThresholdResult check_threshold(const Graph& g,
    ThresholdAlgorithm algo = ThresholdAlgorithm::DEGREE_SEQUENCE) {
    (void)algo;
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

} // namespace graph_recognition

#endif
