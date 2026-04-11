#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H

/**
 * @file triangle_free_enum.h
 * @brief Triangle-free グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き triangle-free グラフを全列挙する。
 *
 * parent(G) = G から最大ラベルの頂点を除去
 * (triangle-free は遺伝的クラスのため常に有効)
 *
 * 頂点を 1, 2, ..., n の順に追加し、各ステップで可能な近傍の
 * すべての部分集合を列挙して triangle-free 判定でフィルタする。
 *
 * 参考文献:
 *   - McKay, J. Algorithms 26, 1998 (geng -t: canonical augmentation)
 *   - Colbourn, Read, J. Graph Theory 3, 1979
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "triangle_free.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Triangle-free 列挙アルゴリズムの選択
 */
enum class TriangleFreeEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Triangle-free 列挙の結果
 */
struct TriangleFreeEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された triangle-free グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct TriangleFreeEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit TriangleFreeEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief Triangle-free 逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。triangle-free でない子を枝刈りする。
 */
inline void triangle_free_enum_dfs(TriangleFreeEnumState& state,
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
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    // pre-extract base edges (between vertices 1..k, excluding x)
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges = base_edges;
        for (int u = 1; u <= k; ++u)
            if (state.adj[x][u])
                edges.push_back(std::make_pair(u, x));
        Graph g(x, edges);
        TriangleFreeResult res = check_triangle_free(g);

        if (res.is_triangle_free) {
            triangle_free_enum_dfs(state, out);
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
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Triangle-free グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return TriangleFreeEnumerationResult
 *
 * 逆探索 (reverse search) を使用。parent(G) は G から最大ラベルの
 * 頂点を除去して得られる。triangle-free は遺伝的クラスの
 * ため、任意の頂点の除去で性質が保存される。
 */
inline TriangleFreeEnumerationResult
enumerate_triangle_free_graphs_reverse_search(int n,
    TriangleFreeEnumAlgorithm algo =
        TriangleFreeEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    TriangleFreeEnumerationResult result;
    if (n < 0) return result;
    detail::TriangleFreeEnumState root(n);
    detail::triangle_free_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
