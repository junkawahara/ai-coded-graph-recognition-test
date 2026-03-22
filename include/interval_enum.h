#ifndef GRAPH_RECOGNITION_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_INTERVAL_ENUM_H

/**
 * @file interval_enum.h
 * @brief インターバルグラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付きインターバルグラフを全列挙する。
 *
 * Interval = chordal ∩ AT-free であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に Interval 性を追加チェックし、非 Interval な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "interval.h"

namespace graph_recognition {

/**
 * @brief Interval 列挙アルゴリズムの選択
 */
enum class IntervalEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Interval 列挙の結果
 */
struct IntervalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Interval グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (interval 用)
 */
inline Graph interval_state_to_graph(const ChordalEnumState& state) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.alive[v]) continue;
            if (state.adj[u][v]) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return Graph(state.total_n, edges);
}

/**
 * @brief Interval 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで Interval 性を検証し
 * 非 Interval な部分木を枝刈りする。
 */
inline void interval_reverse_search_dfs(const ChordalEnumState& state,
                                        std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        Graph g = interval_state_to_graph(children[i]);
        IntervalResult ir = check_interval(g);
        if (!ir.is_interval) continue;

        interval_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Interval グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return IntervalEnumerationResult
 *
 * chordal の逆探索を Interval 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline IntervalEnumerationResult enumerate_interval_graphs_reverse_search(int n,
    IntervalEnumAlgorithm algo = IntervalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    IntervalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::interval_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
