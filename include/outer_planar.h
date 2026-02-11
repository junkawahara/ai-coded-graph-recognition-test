#ifndef GRAPH_RECOGNITION_OUTER_PLANAR_H
#define GRAPH_RECOGNITION_OUTER_PLANAR_H

/**
 * @file outer_planar.h
 * @brief 外平面グラフ (outer planar graph) 認識
 *
 * K4 / K2,3 minor の有無で判定する。
 */

#include "graph.h"
#include "minor.h"

namespace graph_recognition {

/**
 * @brief 外平面グラフ認識アルゴリズムの選択
 */
enum class OuterPlanarAlgorithm {
    MINOR_CHECK /**< マイナーチェック */
};

/**
 * @brief 外平面グラフ認識の結果
 */
struct OuterPlanarResult {
    bool is_outer_planar; /**< 外平面グラフであれば true */
};

/**
 * @brief グラフが外平面グラフか判定する
 * @param g 入力グラフ
 * @return OuterPlanarResult
 */
inline OuterPlanarResult check_outer_planar(const Graph& g,
    OuterPlanarAlgorithm algo = OuterPlanarAlgorithm::MINOR_CHECK) {
    (void)algo;
    OuterPlanarResult res;
    res.is_outer_planar = false;

    int n = g.n;
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    if (n <= 2) {
        res.is_outer_planar = true;
        return res;
    }

    // 単純外平面グラフの辺数上界。
    if (n >= 2 && m > 2LL * n - 3) return res;

    detail_minor::MinorState st = detail_minor::build_minor_state(g);

    detail_minor::MinorChecker k4_checker(detail_minor::MinorTarget::K4);
    if (k4_checker.has_minor(st)) return res;

    detail_minor::MinorChecker k23_checker(detail_minor::MinorTarget::K23);
    if (k23_checker.has_minor(st)) return res;

    res.is_outer_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
