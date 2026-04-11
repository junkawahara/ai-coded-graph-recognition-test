#ifndef GRAPH_RECOGNITION_EULERIAN_H
#define GRAPH_RECOGNITION_EULERIAN_H

/**
 * @file eulerian.h
 * @brief Eulerian グラフ (オイラーグラフ) 認識
 *
 * Eulerian グラフとは、全頂点の次数が偶数であるグラフである。
 * 連結な Eulerian グラフは Euler 回路を持つ。
 *
 * アルゴリズム:
 *   - DEGREE_CHECK: 各頂点の次数の偶奇を検査 O(n)
 *
 * 参考文献:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief Eulerian グラフ認識アルゴリズムの選択
 */
enum class EulerianAlgorithm {
    DEGREE_CHECK /**< 次数偶数検査 O(n) */
};

/**
 * @brief Eulerian グラフ認識の結果
 */
struct EulerianResult {
    bool is_eulerian = false; /**< 全頂点の次数が偶数であれば true */
};

namespace detail {

/**
 * @brief 各頂点の次数が偶数か検査
 *
 * 計算量: O(n)
 */
inline EulerianResult check_eulerian_degree(const Graph& g) {
    EulerianResult res;
    for (int v = 1; v <= g.n; ++v) {
        if (g.adj[v].size() % 2 != 0) {
            return res;
        }
    }
    res.is_eulerian = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフが Eulerian (全頂点の次数が偶数) か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DEGREE_CHECK)
 * @return EulerianResult
 *
 * G が Eulerian ⟺ 全頂点 v について deg(v) が偶数。
 * 連結な場合、Euler 回路が存在する。
 */
inline EulerianResult check_eulerian(const Graph& g,
    EulerianAlgorithm algo = EulerianAlgorithm::DEGREE_CHECK) {
    switch (algo) {
        case EulerianAlgorithm::DEGREE_CHECK:
            return detail::check_eulerian_degree(g);
        default:
            break;
    }
    return EulerianResult();
}

} // namespace graph_recognition

#endif
