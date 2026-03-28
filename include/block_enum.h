#ifndef GRAPH_RECOGNITION_BLOCK_ENUM_H
#define GRAPH_RECOGNITION_BLOCK_ENUM_H

/**
 * @file block_enum.h
 * @brief ブロックグラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付きブロックグラフを全列挙する。
 *
 * Block = chordal かつ各二重連結成分がクリーク (diamond-free chordal)。
 * 遺伝的クラスのため chordal の逆探索木の部分木として列挙できる。
 *
 * parent(G) = G から最大ラベルの simplicial 頂点を除去
 * 子の生成時に Block 性を追加チェックし、非 Block な子を枝刈り。
 *
 * 参考文献:
 *   - Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023
 *   - Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (chordal 列挙)
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "block.h"
#include "chordal_enum.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Block 列挙アルゴリズムの選択
 */
enum class BlockEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Block 列挙の結果
 */
struct BlockEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Block グラフの配列 */
};

namespace detail {

/**
 * @brief ChordalEnumState から Graph を構築する
 */
inline Graph block_state_to_graph(const ChordalEnumState& state) {
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
 * @brief Block 逆探索の DFS
 *
 * chordal の逆探索と同じ構造だが、各ノードで Block 性を検証し
 * 非 Block な部分木を枝刈りする。
 */
inline void block_reverse_search_dfs(const ChordalEnumState& state,
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
        // Block 性チェック: 子が Block でなければ枝刈り
        Graph g = block_state_to_graph(children[i]);
        BlockResult br = check_block(g);
        if (!br.is_block) continue;

        block_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Block グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return BlockEnumerationResult
 *
 * chordal の逆探索を Block 性の枝刈りで拡張。
 * parent(G) は G から最大ラベルの simplicial 頂点を除去して得られる。
 */
inline BlockEnumerationResult enumerate_block_graphs_reverse_search(int n,
    BlockEnumAlgorithm algo = BlockEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BlockEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::block_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
