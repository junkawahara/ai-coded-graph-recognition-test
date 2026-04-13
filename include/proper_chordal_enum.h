#ifndef GRAPH_RECOGNITION_PROPER_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_PROPER_CHORDAL_ENUM_H

/**
 * @file proper_chordal_enum.h
 * @brief Proper chordal グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き proper chordal グラフを全列挙する。
 *
 * Proper chordal = chordal ∩ indifference tree-layout を持つ。
 * 遺伝的クラスのため chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に proper chordal 性を追加チェックし、非 proper chordal な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "proper_chordal.h"

namespace graph_recognition {

/**
 * @brief Proper chordal 列挙アルゴリズムの選択
 */
enum class ProperChordalEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Proper chordal 列挙の結果
 */
struct ProperChordalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された proper chordal グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (proper chordal 用)
 */
inline Graph proper_chordal_state_to_graph(const ChordalEnumState& state) {
    std::vector<int> remap(state.total_n + 1, 0);
    int cnt = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) remap[v] = ++cnt;
    }
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.alive[v]) continue;
            if (state.adj[u][v]) {
                edges.push_back(std::make_pair(remap[u], remap[v]));
            }
        }
    }
    return Graph(cnt, edges);
}

/**
 * @brief Proper chordal 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで proper chordal 性を検証し
 * 非 proper chordal な部分木を枝刈りする。
 */
inline void proper_chordal_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = proper_chordal_state_to_graph(children[i]);
        ProperChordalResult pr = check_proper_chordal(g);
        if (!pr.is_proper_chordal) continue;

        proper_chordal_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き proper chordal グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return ProperChordalEnumerationResult
 *
 * chordal の逆探索を proper chordal 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline ProperChordalEnumerationResult enumerate_proper_chordal_graphs_reverse_search(int n,
    ProperChordalEnumAlgorithm algo = ProperChordalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ProperChordalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::proper_chordal_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
