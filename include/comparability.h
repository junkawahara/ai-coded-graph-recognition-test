#ifndef GRAPH_RECOGNITION_COMPARABILITY_H
#define GRAPH_RECOGNITION_COMPARABILITY_H

/**
 * @file comparability.h
 * @brief 比較可能性グラフ (comparability graph) 認識
 *
 * 推移的向き付けソルバーを用いて比較可能性グラフを認識する。
 */

#include "graph.h"
#include "permutation.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 比較可能性グラフ認識アルゴリズムの選択
 */
enum class ComparabilityAlgorithm {
    MODULAR_DECOMPOSITION /**< モジュラー分解 */
};

/**
 * @brief 比較可能性グラフ認識の結果
 */
struct ComparabilityResult {
    bool is_comparability; /**< 比較可能性グラフであれば true */
};

/**
 * @brief グラフが比較可能性グラフか判定する
 * @param g 入力グラフ
 * @return ComparabilityResult
 *
 * 辺に推移的向き付けが可能であれば比較可能性グラフ。
 */
inline ComparabilityResult check_comparability(const Graph& g,
    ComparabilityAlgorithm algo = ComparabilityAlgorithm::MODULAR_DECOMPOSITION) {
    (void)algo;
    ComparabilityResult res;
    res.is_comparability = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    if (!detail::is_comparability_graph(a)) return res;

    res.is_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
