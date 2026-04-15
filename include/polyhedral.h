#ifndef GRAPH_RECOGNITION_POLYHEDRAL_H
#define GRAPH_RECOGNITION_POLYHEDRAL_H

/**
 * @file polyhedral.h
 * @brief 多面体グラフ (polyhedral graph) 認識
 *
 * Steinitz の定理: 多面体グラフ ⟺ 3-連結平面グラフ。
 */

#include "graph.h"
#include "planar.h"
#include "triconnected.h"

namespace graph_recognition {

/**
 * @brief 多面体グラフ認識アルゴリズムの選択
 */
enum class PolyhedralAlgorithm {
    STEINITZ /**< Steinitz の定理 (3-連結 + 平面) */
};

/**
 * @brief 多面体グラフ認識の結果
 */
struct PolyhedralResult {
    bool is_polyhedral = false; /**< 多面体グラフであれば true */
};

/**
 * @brief グラフが多面体グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: STEINITZ)
 * @return PolyhedralResult
 *
 * Steinitz の定理: 多面体グラフ ⟺ 3-連結かつ平面。
 */
inline PolyhedralResult check_polyhedral(const Graph& g,
    PolyhedralAlgorithm algo = PolyhedralAlgorithm::STEINITZ) {
    (void)algo;
    PolyhedralResult res;

    /* 3-連結チェック (最小次数 3 の事前チェック含む) */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* 平面性チェック */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_polyhedral = true;
    return res;
}

} // namespace graph_recognition

#endif
