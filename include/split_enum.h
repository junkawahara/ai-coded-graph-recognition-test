#ifndef GRAPH_RECOGNITION_SPLIT_ENUM_H
#define GRAPH_RECOGNITION_SPLIT_ENUM_H

/**
 * @file split_enum.h
 * @brief スプリットグラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付きスプリットグラフを全列挙する。
 *
 * Split = chordal ∩ co-chordal であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に Split 性を追加チェックし、非 Split な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "split.h"

namespace graph_recognition {

/**
 * @brief Split 列挙アルゴリズムの選択
 */
enum class SplitEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Split 列挙の結果
 */
struct SplitEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Split グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (split 用)
 */
inline Graph split_state_to_graph(const ChordalEnumState& state) {
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
 * @brief Split 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで Split 性を検証し
 * 非 Split な部分木を枝刈りする。
 */
inline void split_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = split_state_to_graph(children[i]);
        SplitResult sr = check_split(g);
        if (!sr.is_split) continue;

        split_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Split グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return SplitEnumerationResult
 *
 * chordal の逆探索を Split 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline SplitEnumerationResult enumerate_split_graphs_reverse_search(int n,
    SplitEnumAlgorithm algo = SplitEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    SplitEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::split_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
