#ifndef GRAPH_RECOGNITION_CUBIC_H
#define GRAPH_RECOGNITION_CUBIC_H

/**
 * @file cubic.h
 * @brief 三正則グラフ (cubic graph) 認識
 *
 * 全頂点の次数が 3 であることを確認する。
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief 三正則グラフ認識アルゴリズムの選択
 */
enum class CubicAlgorithm {
    DEGREE_CHECK /**< 次数チェック */
};

/**
 * @brief 三正則グラフ認識の結果
 */
struct CubicResult {
    bool is_cubic = false; /**< 三正則グラフであれば true */
};

/**
 * @brief グラフが三正則グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: DEGREE_CHECK)
 * @return CubicResult
 *
 * 三正則グラフ ⟺ 全頂点の次数が 3。
 */
inline CubicResult check_cubic(const Graph& g,
    CubicAlgorithm algo = CubicAlgorithm::DEGREE_CHECK) {
    (void)algo;
    CubicResult res;

    int n = g.n;
    if (n == 0) return res;

    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    res.is_cubic = true;
    return res;
}

} // namespace graph_recognition

#endif
