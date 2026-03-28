#ifndef GRAPH_RECOGNITION_OUTER_PLANAR_H
#define GRAPH_RECOGNITION_OUTER_PLANAR_H

/**
 * @file outer_planar.h
 * @brief 外平面グラフ (outer planar graph) 認識
 *
 * G が外平面グラフ ⟺ G + K₁ (全頂点に接続する新頂点を追加) が平面グラフ。
 * 平面性判定には de Fraysseix-Rosenstiehl のインターレースメント法を使用。
 *
 * フォールバックとして K4 / K2,3 minor チェックも利用可能。
 */

#include "graph.h"
#include "minor.h"
#include "planar.h"

#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 外平面グラフ認識アルゴリズムの選択
 */
enum class OuterPlanarAlgorithm {
    AUGMENTED_PLANARITY, /**< G + K₁ の平面性判定 */
    MINOR_CHECK          /**< マイナーチェック (指数時間) */
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
 * @param algo 使用アルゴリズム (デフォルト: AUGMENTED_PLANARITY)
 * @return OuterPlanarResult
 */
inline OuterPlanarResult check_outer_planar(const Graph& g,
    OuterPlanarAlgorithm algo = OuterPlanarAlgorithm::AUGMENTED_PLANARITY) {
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

    if (algo == OuterPlanarAlgorithm::MINOR_CHECK) {
        detail_minor::MinorState st = detail_minor::build_minor_state(g);

        detail_minor::MinorChecker k4_checker(detail_minor::MinorTarget::K4);
        if (k4_checker.has_minor(st)) return res;

        detail_minor::MinorChecker k23_checker(detail_minor::MinorTarget::K23);
        if (k23_checker.has_minor(st)) return res;

        res.is_outer_planar = true;
        return res;
    }

    // AUGMENTED_PLANARITY: G + K₁ を構築して平面性判定
    // 新頂点 n+1 を全既存頂点に接続
    std::vector<std::pair<int, int>> edges;
    edges.reserve(static_cast<size_t>(m + n));
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int w = g.adj[v][i];
            if (v < w) {
                edges.push_back(std::make_pair(v, w));
            }
        }
    }
    int new_v = n + 1;
    for (int v = 1; v <= n; ++v) {
        edges.push_back(std::make_pair(new_v, v));
    }

    Graph augmented(new_v, edges);
    PlanarResult pr = check_planar(augmented, PlanarAlgorithm::LR_INTERLACEMENT);
    res.is_outer_planar = pr.is_planar;
    return res;
}

} // namespace graph_recognition

#endif
