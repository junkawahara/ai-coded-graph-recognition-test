#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H

/**
 * @file proper_interval_enum.h
 * @brief 固有インターバルグラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き固有インターバルグラフを全列挙する。
 *
 * Proper interval = interval ∩ claw-free であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に proper interval 性を追加チェックし、非 proper interval な子を枝刈り。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "proper_interval.h"

namespace graph_recognition {

/**
 * @brief Proper interval 列挙アルゴリズムの選択
 */
enum class ProperIntervalEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Proper interval 列挙の結果
 */
struct ProperIntervalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された proper interval グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する (proper interval 用)
 */
inline Graph proper_interval_state_to_graph(const ChordalEnumState& state) {
    // Remap alive vertices to [1..alive_count] to avoid dead vertex overhead
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
 * @brief Proper interval 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで proper interval 性を検証し
 * 非 proper interval な部分木を枝刈りする。
 */
inline void proper_interval_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = proper_interval_state_to_graph(children[i]);
        ProperIntervalResult pr = check_proper_interval(g);
        if (!pr.is_proper_interval) continue;

        proper_interval_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き proper interval グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return ProperIntervalEnumerationResult
 *
 * chordal の逆探索を proper interval 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline ProperIntervalEnumerationResult enumerate_proper_interval_graphs_reverse_search(int n,
    ProperIntervalEnumAlgorithm algo = ProperIntervalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ProperIntervalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::proper_interval_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
