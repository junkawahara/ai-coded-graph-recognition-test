#ifndef GRAPH_RECOGNITION_STRONGLY_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_STRONGLY_CHORDAL_ENUM_H

/**
 * @file strongly_chordal_enum.h
 * @brief 強弦グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き強弦グラフを全列挙する。
 *
 * Strongly chordal = chordal かつ strong elimination ordering を持つ。
 * 遺伝的クラスのため chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に強弦性を追加チェックし、非強弦な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "strongly_chordal.h"

namespace graph_recognition {

/**
 * @brief 強弦グラフ列挙アルゴリズムの選択
 */
enum class StronglyChordalEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief 強弦グラフ列挙の結果
 */
struct StronglyChordalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された強弦グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (strongly chordal 用)
 */
inline Graph strongly_chordal_state_to_graph(const ChordalEnumState& state) {
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
 * @brief 強弦グラフ逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで強弦性を検証し
 * 非強弦な部分木を枝刈りする。
 */
inline void strongly_chordal_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = strongly_chordal_state_to_graph(children[i]);
        StronglyChordalResult sr = check_strongly_chordal(g);
        if (!sr.is_strongly_chordal) continue;

        strongly_chordal_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き強弦グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return StronglyChordalEnumerationResult
 *
 * chordal の逆探索を強弦性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline StronglyChordalEnumerationResult enumerate_strongly_chordal_graphs_reverse_search(int n,
    StronglyChordalEnumAlgorithm algo = StronglyChordalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    StronglyChordalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::strongly_chordal_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
