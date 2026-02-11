#ifndef GRAPH_RECOGNITION_SPLIT_H
#define GRAPH_RECOGNITION_SPLIT_H

/**
 * @file split.h
 * @brief スプリットグラフ (split graph) 認識
 *
 * G と補グラフの両方が弦グラフであればスプリットグラフと判定する。
 */

#include "chordal.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief スプリットグラフ認識アルゴリズムの選択
 */
enum class SplitAlgorithm {
    DEGREE_SEQUENCE /**< 次数列による判定 */
};

/**
 * @brief スプリットグラフ認識の結果
 */
struct SplitResult {
    bool is_split; /**< スプリットグラフであれば true */
};

namespace detail {

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

} // namespace detail

/**
 * @brief グラフがスプリットグラフか判定する
 * @param g 入力グラフ
 * @return SplitResult
 *
 * G がスプリットグラフ ⟺ G と complement(G) がともに弦グラフ。
 */
inline SplitResult check_split(const Graph& g,
    SplitAlgorithm algo = SplitAlgorithm::DEGREE_SEQUENCE) {
    (void)algo;
    SplitResult res;
    res.is_split = false;

    ChordalResult g_chordal = check_chordal(g);
    if (!g_chordal.is_chordal) return res;

    Graph gc = detail::build_complement_graph(g);
    ChordalResult gc_chordal = check_chordal(gc);
    if (!gc_chordal.is_chordal) return res;

    res.is_split = true;
    return res;
}

} // namespace graph_recognition

#endif
