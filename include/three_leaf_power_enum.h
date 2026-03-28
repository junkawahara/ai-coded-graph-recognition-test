#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H

/**
 * @file three_leaf_power_enum.h
 * @brief 3-leaf power グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き 3-leaf power グラフを全列挙する。
 *
 * 3-leaf power ⊂ Ptolemaic ⊂ Chordal であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に 3-leaf power 性を追加チェックし、非 3-leaf power な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "three_leaf_power.h"

namespace graph_recognition {

/**
 * @brief 3-leaf power 列挙の結果
 */
struct ThreeLeafPowerEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された 3-leaf power グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する
 */
inline Graph tlp_state_to_graph(const ChordalEnumState& state) {
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
 * @brief 3-leaf power 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで 3-leaf power 性を検証し
 * 非 3-leaf power な部分木を枝刈りする。
 */
inline void three_leaf_power_reverse_search_dfs(const ChordalEnumState& state,
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
        // 3-leaf power 性チェック: 子が 3-leaf power でなければ枝刈り
        Graph g = tlp_state_to_graph(children[i]);
        ThreeLeafPowerResult tr = check_three_leaf_power(g);
        if (!tr.is_three_leaf_power) continue;

        three_leaf_power_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き 3-leaf power グラフを全列挙する
 * @param n 頂点数
 * @return ThreeLeafPowerEnumerationResult
 *
 * chordal の逆探索を 3-leaf power 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline ThreeLeafPowerEnumerationResult enumerate_three_leaf_power_graphs_reverse_search(int n) {
    ThreeLeafPowerEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::three_leaf_power_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
