#ifndef GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H
#define GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H

/**
 * @file proper_circular_arc.h
 * @brief 固有円弧グラフ (proper circular-arc graph) 認識
 *
 * 固有円弧グラフとは、円弧グラフのうち、どの弧も他の弧を真に含まない
 * 表現を持つグラフである。
 *
 * 特徴づけ: G が固有円弧グラフ ⟺ G が円弧グラフかつ claw-free (K_{1,3}-free)
 *
 * これは固有インターバルグラフ (interval ∩ claw-free) の類似。
 *
 * 参考文献:
 *   - Tucker (1974), "Structure theorems for some circular-arc graphs"
 *   - Deng, Hell, Huang (1996), O(n+m) 認識
 *   - Lin, Soulignac, Szwarcfiter (2013), certifying 線形時間
 */

#include "graph.h"
#include "circular_arc.h"
#include "claw_free.h"

namespace graph_recognition {

/**
 * @brief 固有円弧グラフ認識の結果
 */
struct ProperCircularArcResult {
    bool is_proper_circular_arc = false; /**< 固有円弧グラフであれば true */
};

/**
 * @brief グラフが固有円弧グラフか判定する
 * @param g 入力グラフ
 * @return ProperCircularArcResult
 *
 * G が固有円弧グラフ ⟺ G が円弧グラフかつ claw-free。
 */
inline ProperCircularArcResult check_proper_circular_arc(const Graph& g) {
    ProperCircularArcResult res;

    CircularArcResult ca = check_circular_arc(g);
    if (!ca.is_circular_arc) return res;

    ClawFreeResult cf = check_claw_free(g);
    if (!cf.is_claw_free) return res;

    res.is_proper_circular_arc = true;
    return res;
}

}  // namespace graph_recognition

#endif
