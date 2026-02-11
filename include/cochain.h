#ifndef GRAPH_RECOGNITION_COCHAIN_H
#define GRAPH_RECOGNITION_COCHAIN_H

/**
 * @file cochain.h
 * @brief 余チェーングラフ (cochain graph) 認識
 *
 * 補グラフがチェーングラフであれば余チェーングラフと判定する。
 */

#include "chain.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 余チェーングラフ認識アルゴリズムの選択
 */
enum class CochainAlgorithm {
    COMPLEMENT /**< 補グラフのチェーン判定 */
};

/**
 * @brief 余チェーングラフ認識の結果
 */
struct CochainResult {
    bool is_cochain; /**< 余チェーングラフであれば true */
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
 * @brief グラフが余チェーングラフか判定する
 * @param g 入力グラフ
 * @return CochainResult
 *
 * G が余チェーングラフ ⟺ complement(G) がチェーングラフ。
 */
inline CochainResult check_cochain(const Graph& g,
    CochainAlgorithm algo = CochainAlgorithm::COMPLEMENT) {
    (void)algo;
    CochainResult res;
    res.is_cochain = false;

    Graph gc = detail::build_complement_graph(g);
    ChainResult cres = check_chain(gc);
    if (!cres.is_chain) return res;

    res.is_cochain = true;
    return res;
}

} // namespace graph_recognition

#endif
