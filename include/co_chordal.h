#ifndef GRAPH_RECOGNITION_CO_CHORDAL_H
#define GRAPH_RECOGNITION_CO_CHORDAL_H

/**
 * @file co_chordal.h
 * @brief 余弦グラフ (co-chordal graph) 認識
 *
 * 補グラフが弦グラフであれば余弦グラフと判定する。
 */

#include "chordal.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 余弦グラフ認識アルゴリズムの選択
 */
enum class CoChordalAlgorithm {
    COMPLEMENT /**< 補グラフの弦性判定 */
};

/**
 * @brief 余弦グラフ認識の結果
 */
struct CoChordalResult {
    bool is_co_chordal; /**< 余弦グラフであれば true */
};

namespace detail_co_chordal {

/** @brief 補グラフを構築する (内部関数) */
inline Graph build_complement_graph(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

} // namespace detail_co_chordal

/**
 * @brief グラフが余弦グラフか判定する
 * @param g 入力グラフ
 * @return CoChordalResult
 *
 * G が余弦グラフ ⟺ complement(G) が弦グラフ。
 */
inline CoChordalResult check_co_chordal(const Graph& g,
    CoChordalAlgorithm algo = CoChordalAlgorithm::COMPLEMENT) {
    (void)algo;
    CoChordalResult res;
    res.is_co_chordal = false;

    Graph gc = detail_co_chordal::build_complement_graph(g);
    ChordalResult cres = check_chordal(gc);
    if (!cres.is_chordal) return res;

    res.is_co_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
