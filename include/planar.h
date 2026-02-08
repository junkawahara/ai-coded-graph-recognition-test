#ifndef GRAPH_RECOGNITION_PLANAR_H
#define GRAPH_RECOGNITION_PLANAR_H

/**
 * @file planar.h
 * @brief 平面グラフ (planar graph) 認識
 *
 * Kuratowski の定理に基づき、K5 / K3,3 minor の有無で判定する。
 */

#include "graph.h"
#include "minor.h"

namespace graph_recognition {

/**
 * @brief 平面グラフ認識の結果
 */
struct PlanarResult {
    bool is_planar; /**< 平面グラフであれば true */
};

/**
 * @brief グラフが平面グラフか判定する
 * @param g 入力グラフ
 * @return PlanarResult
 */
inline PlanarResult check_planar(const Graph& g) {
    PlanarResult res;
    res.is_planar = false;

    int n = g.n;
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    if (n <= 4) {
        res.is_planar = true;
        return res;
    }

    // 単純平面グラフの辺数上界。
    if (n >= 3 && m > 3LL * n - 6) return res;

    detail_minor::MinorState st = detail_minor::build_minor_state(g);

    detail_minor::MinorChecker k5_checker(detail_minor::MinorTarget::K5);
    if (k5_checker.has_minor(st)) return res;

    detail_minor::MinorChecker k33_checker(detail_minor::MinorTarget::K33);
    if (k33_checker.has_minor(st)) return res;

    res.is_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
