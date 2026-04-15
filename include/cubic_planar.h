#ifndef GRAPH_RECOGNITION_CUBIC_PLANAR_H
#define GRAPH_RECOGNITION_CUBIC_PLANAR_H

/**
 * @file cubic_planar.h
 * @brief 三正則平面グラフ (cubic planar graph) 認識
 *
 * 三正則性と平面性の両方を確認する。
 */

#include "graph.h"
#include "cubic.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief 三正則平面グラフ認識アルゴリズムの選択
 */
enum class CubicPlanarAlgorithm {
    CUBIC_AND_PLANAR /**< 三正則 + 平面性チェック */
};

/**
 * @brief 三正則平面グラフ認識の結果
 */
struct CubicPlanarResult {
    bool is_cubic_planar = false; /**< 三正則平面グラフであれば true */
};

/**
 * @brief グラフが三正則平面グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: CUBIC_AND_PLANAR)
 * @return CubicPlanarResult
 *
 * 三正則平面グラフ ⟺ 全頂点の次数が 3 かつ平面。
 */
inline CubicPlanarResult check_cubic_planar(const Graph& g,
    CubicPlanarAlgorithm algo = CubicPlanarAlgorithm::CUBIC_AND_PLANAR) {
    (void)algo;
    CubicPlanarResult res;

    /* 三正則チェック (高速に失敗可能) */
    CubicResult cr = check_cubic(g);
    if (!cr.is_cubic) return res;

    /* 平面性チェック */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_cubic_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
