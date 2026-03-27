#ifndef GRAPH_RECOGNITION_BIPARTITE_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_ENUM_H

/**
 * @file bipartite_enum.h
 * @brief 二部グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き二部グラフを全列挙する。
 *
 * parent(G) = G から最大ラベルの頂点を除去
 * (二部グラフは遺伝的クラスのため常に有効)
 *
 * 頂点を 1, 2, ..., n の順に追加し、各ステップで可能な近傍の
 * すべての部分集合を列挙して二部グラフ判定でフィルタする。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "bipartite.h"
#include "chordal_enum.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief 二部グラフ列挙アルゴリズムの選択
 */
enum class BipartiteEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief 二部グラフ列挙の結果
 */
struct BipartiteEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された二部グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct BipartiteEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit BipartiteEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief 二部グラフ逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。二部グラフでない子を枝刈りする。
 */
inline void bipartite_enum_dfs(BipartiteEnumState& state,
                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= x; ++u)
            for (int v = u + 1; v <= x; ++v)
                if (state.adj[u][v])
                    edges.push_back(std::make_pair(u, v));
        Graph g(x, edges);
        BipartiteResult res = check_bipartite(g);

        if (res.is_bipartite) {
            bipartite_enum_dfs(state, out);
        }

        for (int u = 1; u <= k; ++u) {
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き二部グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return BipartiteEnumerationResult
 *
 * 逆探索 (reverse search) を使用。parent(G) は G から最大ラベルの
 * 頂点を除去して得られる。二部グラフは遺伝的クラスの
 * ため、任意の頂点の除去で性質が保存される。
 */
inline BipartiteEnumerationResult
enumerate_bipartite_graphs_reverse_search(int n,
    BipartiteEnumAlgorithm algo =
        BipartiteEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BipartiteEnumerationResult result;
    if (n < 0) return result;
    detail::BipartiteEnumState root(n);
    detail::bipartite_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
