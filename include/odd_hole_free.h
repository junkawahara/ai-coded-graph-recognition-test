#ifndef GRAPH_RECOGNITION_ODD_HOLE_FREE_H
#define GRAPH_RECOGNITION_ODD_HOLE_FREE_H

/**
 * @file odd_hole_free.h
 * @brief Odd-hole-free グラフ認識
 *
 * Odd-hole-free グラフとは、長さ 5 以上の誘導奇数閉路 (odd hole) を
 * 含まないグラフである。
 *
 * perfect = odd-hole-free ∩ odd-antihole-free (Strong Perfect Graph Theorem)
 *
 * アルゴリズ��:
 *   perfect.h の has_odd_hole() を利用。各辺 (u,v) について制限グラフ上の
 *   BFS + DFS で奇数穴を検出する。
 *
 * 参考文献:
 *   - Chudnovsky, Scott, Seymour, Spirkl, JACM 67(1), 2020
 */

#include "graph.h"
#include "perfect.h"

namespace graph_recognition {

/**
 * @brief Odd-hole-free グラフ認識の結果
 */
struct OddHoleFreeResult {
    bool is_odd_hole_free = false; /**< odd-hole-free であれば true */
};

/**
 * @brief グラフが odd-hole-free か判定する
 * @param g 入力グラ��
 * @return OddHoleFreeResult
 */
inline OddHoleFreeResult check_odd_hole_free(const Graph& g) {
    OddHoleFreeResult res;
    if (g.n <= 4) {
        res.is_odd_hole_free = true;
        return res;
    }
    res.is_odd_hole_free = !detail_perfect::has_odd_hole(g);
    return res;
}

} // namespace graph_recognition

#endif
