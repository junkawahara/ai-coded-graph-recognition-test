#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H

/**
 * @file maximal_planar_enum.h
 * @brief 極大平面グラフ (maximal planar graph / triangulation) の列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き極大平面グラフを全列挙する。
 *
 * 極大平面グラフ: 平面グラフで、辺を追加すると平面性が失われるもの。
 * n >= 3 では辺数 = 3n-6 と同値。全ての面が三角形（三角形分割）。
 *
 * parent(G) = G から最大ラベルの頂点を除去
 * (planar は遺伝的クラスのため常に有効)
 *
 * 頂点を 1, 2, ..., n の順に追加し、各ステップで可能な近傍の
 * すべての部分集合を列挙して planar 判定でフィルタする。
 * 最終ステップで辺数 = target_edges のもののみ出力する。
 *
 * 枝刈り: 各ステップで新頂点の次数の上下界を計算し、
 * 到達不可能な近傍パターンをスキップする。
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief Maximal Planar 列挙アルゴリズムの選択
 */
enum class MaximalPlanarEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief Maximal Planar 列挙の結果
 */
struct MaximalPlanarEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された極大平面グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct MaximalPlanarEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    int target_edges; /**< 極大平面グラフに必要な辺数 */

    explicit MaximalPlanarEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          target_edges(n >= 3 ? 3 * n - 6 : n * (n - 1) / 2) {}
};

/**
 * @brief Maximal Planar 逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。planar でない子を枝刈りし、
 * 最終ステップで辺数 = target_edges のもののみ出力する。
 */
inline void maximal_planar_enum_dfs(MaximalPlanarEnumState& state,
                                    std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        int edge_count = 0;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    ++edge_count;
        if (edge_count != state.target_edges) return;

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

    // 現在の辺数を計算
    int current_edges = 0;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                ++current_edges;

    // 頂点 x 以降に追加可能な辺数の上界（x を除く）
    // remaining_capacity_after_x = sum_{j=x+1}^{n} (j-1)
    long long remaining_after_x = 0;
    for (int j = x + 1; j <= state.total_n; ++j)
        remaining_after_x += (j - 1);

    // 新頂点 x の次数の下界と上界
    int d_min_raw = static_cast<int>(
        state.target_edges - current_edges - remaining_after_x);
    int d_min = (d_min_raw > 0) ? d_min_raw : 0;
    int d_max = state.target_edges - current_edges;
    if (d_max > k) d_max = k;

    // 到達不可能な場合は枝刈り
    if (d_min > k || d_max < 0) return;

    // base edges を事前抽出
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // popcount による枝刈り
        int deg = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) {
                tmp &= (tmp - 1);
                ++deg;
            }
        }
        if (deg < d_min || deg > d_max) continue;

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
        PlanarResult res = check_planar(g);

        if (res.is_planar) {
            maximal_planar_enum_dfs(state, out);
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
 * @brief 頂点集合 {1, ..., n} 上のラベル付き極大平面グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return MaximalPlanarEnumerationResult
 *
 * 逆探索 (reverse search) を使用。parent(G) は G から最大ラベルの
 * 頂点を除去して得られる。planar は遺伝的クラスの
 * ため、任意の頂点の除去で性質が保存される。
 * 最終ステップで辺数 = 3n-6 (n>=3) のもののみ出力する。
 */
inline MaximalPlanarEnumerationResult
enumerate_maximal_planar_graphs_reverse_search(int n,
    MaximalPlanarEnumAlgorithm algo =
        MaximalPlanarEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    MaximalPlanarEnumerationResult result;
    if (n < 0) return result;
    detail::MaximalPlanarEnumState root(n);
    detail::maximal_planar_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
