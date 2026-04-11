#ifndef GRAPH_RECOGNITION_FIVE_LEAF_POWER_ENUM_H
#define GRAPH_RECOGNITION_FIVE_LEAF_POWER_ENUM_H

/**
 * @file five_leaf_power_enum.h
 * @brief 5-leaf power グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き 5-leaf power グラフを全列挙する。
 *
 * 5-leaf power ⊂ Strongly Chordal ⊂ Chordal であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に 5-leaf power 性を追加チェックし、非 5-leaf power な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "five_leaf_power.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief 5-leaf power 列挙の結果
 */
struct FiveLeafPowerEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された 5-leaf power グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (5-leaf power 用)
 */
inline Graph fivlp_state_to_graph(const ChordalEnumState& state) {
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
 * @brief 5-leaf power 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで 5-leaf power 性を検証し
 * 非 5-leaf power な部分木を枝刈りする。
 */
inline void five_leaf_power_reverse_search_dfs(const ChordalEnumState& state,
                                                std::vector<EnumeratedGraph>* out) {
    // 全頂点が生きている → 完成したグラフ
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    // 子を生成（chordal の逆探索と同じ）
    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        // 5-leaf power 性チェック: 子が 5-leaf power でなければ枝刈り
        Graph g = fivlp_state_to_graph(children[i]);
        FiveLeafPowerResult fr = check_five_leaf_power(g);
        if (!fr.is_five_leaf_power) continue;

        five_leaf_power_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き 5-leaf power グラフを全列挙する
 * @param n 頂点数
 * @return FiveLeafPowerEnumerationResult
 *
 * chordal の逆探索を 5-leaf power 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline FiveLeafPowerEnumerationResult enumerate_five_leaf_power_graphs_reverse_search(int n) {
    FiveLeafPowerEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::five_leaf_power_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
