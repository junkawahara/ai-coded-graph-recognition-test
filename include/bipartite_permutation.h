#ifndef GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_H
#define GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_H

/**
 * @file bipartite_permutation.h
 * @brief 二部順列グラフ (bipartite permutation graph) 認識
 *
 * 二部順列グラフは、二部グラフかつ順列グラフであるグラフ。
 */

#include "bipartite.h"
#include "graph.h"
#include "permutation.h"

namespace graph_recognition {

/**
 * @brief 二部順列グラフ認識アルゴリズムの選択
 */
enum class BipartitePermutationAlgorithm {
    CHAIN_BOTH_SIDES /**< 両側チェーン判定 */
};

/**
 * @brief 二部順列グラフ認識の結果
 */
struct BipartitePermutationResult {
    bool is_bipartite_permutation; /**< 二部順列グラフであれば true */
};

/**
 * @brief グラフが二部順列グラフか判定する
 * @param g 入力グラフ
 * @return BipartitePermutationResult
 *
 * G が二部順列グラフ ⟺ G が二部グラフかつ順列グラフ。
 */
inline BipartitePermutationResult check_bipartite_permutation(const Graph& g,
    BipartitePermutationAlgorithm algo = BipartitePermutationAlgorithm::CHAIN_BOTH_SIDES) {
    (void)algo;
    BipartitePermutationResult res;
    res.is_bipartite_permutation = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    PermutationResult perm = check_permutation(g);
    if (!perm.is_permutation) return res;

    res.is_bipartite_permutation = true;
    return res;
}

} // namespace graph_recognition

#endif
