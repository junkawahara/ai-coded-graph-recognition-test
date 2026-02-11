#ifndef GRAPH_RECOGNITION_CO_COMPARABILITY_H
#define GRAPH_RECOGNITION_CO_COMPARABILITY_H

/**
 * @file co_comparability.h
 * @brief 余比較可能性グラフ (co-comparability graph) 認識
 *
 * 補グラフが比較可能性グラフであれば余比較可能性グラフと判定する。
 */

#include "comparability.h"
#include "graph.h"
#include "permutation.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 余比較可能性グラフ認識アルゴリズムの選択
 */
enum class CoComparabilityAlgorithm {
    COMPLEMENT /**< 補グラフの比較可能性判定 */
};

/**
 * @brief 余比較可能性グラフ認識の結果
 */
struct CoComparabilityResult {
    bool is_co_comparability; /**< 余比較可能性グラフであれば true */
};

/**
 * @brief グラフが余比較可能性グラフか判定する
 * @param g 入力グラフ
 * @return CoComparabilityResult
 *
 * G が余比較可能性グラフ ⟺ complement(G) が比較可能性グラフ。
 */
inline CoComparabilityResult check_co_comparability(const Graph& g,
    CoComparabilityAlgorithm algo = CoComparabilityAlgorithm::COMPLEMENT) {
    (void)algo;
    CoComparabilityResult res;
    res.is_co_comparability = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);
    if (!detail::is_comparability_graph(c)) return res;

    res.is_co_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
