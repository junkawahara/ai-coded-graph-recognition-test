#ifndef GRAPH_RECOGNITION_AT_FREE_H
#define GRAPH_RECOGNITION_AT_FREE_H

/**
 * @file at_free.h
 * @brief AT-free グラフ認識
 *
 * 小惑星三つ組 (asteroidal triple) が存在しなければ AT-free と判定する。
 */

#include "graph.h"
#include "interval.h"

namespace graph_recognition {

/**
 * @brief AT-free グラフ認識アルゴリズムの選択
 */
enum class ATFreeAlgorithm {
    BRUTE_FORCE /**< 全三つ組の探索 */
};

/**
 * @brief AT-free グラフ認識の結果
 */
struct ATFreeResult {
    bool is_at_free; /**< AT-free であれば true */
};

/**
 * @brief グラフが AT-free か判定する
 * @param g 入力グラフ
 * @return ATFreeResult
 *
 * 小惑星三つ組 (asteroidal triple) が存在しなければ AT-free。
 */
inline ATFreeResult check_at_free(const Graph& g,
    ATFreeAlgorithm algo = ATFreeAlgorithm::BRUTE_FORCE) {
    (void)algo;
    ATFreeResult res;
    res.is_at_free = !detail::has_asteroidal_triple(g);
    return res;
}

} // namespace graph_recognition

#endif
