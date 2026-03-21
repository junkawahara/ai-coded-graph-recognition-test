#ifndef GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H
#define GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H

/**
 * @file ptolemaic_enum.h
 * @brief プトレマイオスグラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付きプトレマイオスグラフを全列挙する。
 *
 * Ptolemaic = chordal ∩ distance-hereditary であり、遺伝的クラスのため
 * chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に Ptolemaic 性を追加チェックし、非 Ptolemaic な子を枝刈り。
 *
 * 参考文献:
 *   - Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023
 *   - Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (chordal 列挙)
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "ptolemaic.h"

namespace graph_recognition {

/**
 * @brief Ptolemaic 列挙アルゴリズムの選択
 */
enum class PtolemaicEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Ptolemaic 列挙の結果
 */
struct PtolemaicEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Ptolemaic グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する
 */
inline Graph state_to_graph(const ChordalEnumState& state) {
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
 * @brief Ptolemaic 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで Ptolemaic 性を検証し
 * 非 Ptolemaic な部分木を枝刈りする。
 */
inline void ptolemaic_reverse_search_dfs(const ChordalEnumState& state,
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
        // Ptolemaic 性チェック: 子が Ptolemaic でなければ枝刈り
        Graph g = state_to_graph(children[i]);
        PtolemaicResult pr = check_ptolemaic(g);
        if (!pr.is_ptolemaic) continue;

        ptolemaic_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Ptolemaic グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return PtolemaicEnumerationResult
 *
 * chordal の逆探索を Ptolemaic 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline PtolemaicEnumerationResult enumerate_ptolemaic_graphs_reverse_search(int n,
    PtolemaicEnumAlgorithm algo = PtolemaicEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    PtolemaicEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::ptolemaic_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
