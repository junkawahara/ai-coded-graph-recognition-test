#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_H

/**
 * @file maximal_planar.h
 * @brief 極大平面グラフ (maximal planar graph) 認識
 *
 * 平面性チェックと辺数 m = 3n - 6 の確認により判定する。
 */

#include "graph.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief 極大平面グラフ認識アルゴリズムの選択
 */
enum class MaximalPlanarAlgorithm {
    PLANAR_EDGE_COUNT /**< 平面性 + 辺数チェック */
};

/**
 * @brief 極大平面グラフ認識の結果
 */
struct MaximalPlanarResult {
    bool is_maximal_planar = false; /**< 極大平面グラフであれば true */
};

/**
 * @brief グラフが極大平面グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: PLANAR_EDGE_COUNT)
 * @return MaximalPlanarResult
 *
 * 極大平面グラフ ⟺ 平面かつ m = 3n - 6 (n >= 3)。
 * n <= 2 の場合: K1, K2 は極大平面。
 */
inline MaximalPlanarResult check_maximal_planar(const Graph& g,
    MaximalPlanarAlgorithm algo = MaximalPlanarAlgorithm::PLANAR_EDGE_COUNT) {
    (void)algo;
    MaximalPlanarResult res;

    int n = g.n;

    /* n <= 2: K0 (空), K1, K2 は極大平面 */
    if (n <= 2) {
        long long m = 0;
        for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
        m /= 2;
        /* n=0: m=0 → 極大平面 */
        /* n=1: m=0 → 極大平面 */
        /* n=2: m=1 → 極大平面, m=0 → 非極大 (辺を追加可能) */
        if (n == 0 || n == 1) {
            res.is_maximal_planar = true;
        } else if (n == 2 && m == 1) {
            res.is_maximal_planar = true;
        }
        return res;
    }

    /* 辺数チェック: m = 3n - 6 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 3LL * n - 6) return res;

    /* 平面性チェック */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_maximal_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
