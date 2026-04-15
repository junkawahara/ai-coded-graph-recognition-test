#ifndef GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_H
#define GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_H

/**
 * @file simple_quadrangulation.h
 * @brief 単純四角分割 (simple quadrangulation) 認識
 *
 * 3-連結 + 平面 + m = 2n - 4 + 三角形フリー で判定する。
 * 3-連結平面グラフでは Whitney の定理により埋め込みが一意。
 * m = 2n - 4 なら平均面サイズ = 4 であり、三角形がなければ全面が四角形。
 */

#include "graph.h"
#include "planar.h"
#include "triconnected.h"
#include "triangle_free.h"

namespace graph_recognition {

/**
 * @brief 単純四角分割認識アルゴリズムの選択
 */
enum class SimpleQuadrangulationAlgorithm {
    COMBINED /**< 3-連結 + 平面 + 辺数 + 三角形フリー */
};

/**
 * @brief 単純四角分割認識の結果
 */
struct SimpleQuadrangulationResult {
    bool is_simple_quadrangulation = false; /**< 単純四角分割であれば true */
};

/**
 * @brief グラフが単純四角分割か判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: COMBINED)
 * @return SimpleQuadrangulationResult
 *
 * 単純四角分割 ⟺ 3-連結平面グラフで全面が四角形。
 * 判定: 3-連結 ∧ 平面 ∧ m = 2n - 4 ∧ 三角形フリー。
 */
inline SimpleQuadrangulationResult check_simple_quadrangulation(const Graph& g,
    SimpleQuadrangulationAlgorithm algo = SimpleQuadrangulationAlgorithm::COMBINED) {
    (void)algo;
    SimpleQuadrangulationResult res;

    int n = g.n;
    if (n < 4) return res; /* 最小の四角分割は K_{2,2} (= C4), n=4 */

    /* 辺数チェック: m = 2n - 4 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 2LL * n - 4) return res;

    /* 三角形フリーチェック (高速フィルタ) */
    TriangleFreeResult tfr = check_triangle_free(g);
    if (!tfr.is_triangle_free) return res;

    /* 3-連結チェック */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* 平面性チェック */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_simple_quadrangulation = true;
    return res;
}

} // namespace graph_recognition

#endif
