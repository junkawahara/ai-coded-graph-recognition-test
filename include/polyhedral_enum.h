#ifndef GRAPH_RECOGNITION_POLYHEDRAL_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_ENUM_H

/**
 * @file polyhedral_enum.h
 * @brief 多面体グラフ (3-連結平面グラフ) の列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き 3-連結平面グラフを全列挙する。
 *
 * parent(G) = G から最大ラベルの頂点を除去
 *
 * 3-連結は遺伝的 (hereditary) でないため、中間ステップでは
 * 以下の枝刈りを適用し、最終ステップで完全判定を行う:
 *   1. 平面性枝刈り (遺伝的: 非平面ならどの拡張も非平面)
 *   2. 連結性枝刈り: 連結成分数 > 残り頂点数 + 1 なら枝刈り
 *   3. 次数下限枝刈り: 最終2レベルで各頂点の次数 >= 3 制約をチェック
 *   4. 最終ステップでの完全 3-連結 + 平面判定
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "planar.h"
#include "triconnected.h"

namespace graph_recognition {

/**
 * @brief 多面体列挙アルゴリズムの選択
 */
enum class PolyhedralEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief 多面体列挙の結果
 */
struct PolyhedralEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された多面体グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct PolyhedralEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    std::vector<int> deg;  /**< 各頂点の次数 */

    explicit PolyhedralEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief {1, ..., x} 上の連結成分数を数える
 */
inline int polyhedral_count_components(const PolyhedralEnumState& state,
                                       int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (int u = 1; u <= x; ++u) {
                if (!visited[u] && state.adj[v][u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
    }
    return comp;
}

/**
 * @brief 多面体逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。平面性 (遺伝的) で中間枝刈りし、
 * 最終ステップで 3-連結 + 平面判定を行う。
 */
inline void polyhedral_enum_dfs(PolyhedralEnumState& state,
                                 std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // 最終ステップ: 完全な平面性 + 3-連結判定
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    edges.push_back(std::make_pair(u, v));

        Graph g(state.total_n, edges);
        PlanarResult pr = check_planar(g);
        if (!pr.is_planar) return;

        TriconnectedResult tr = check_triconnected(g);
        if (tr.is_triconnected) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            graph.edges = edges;
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;

    // 中間ステップ用: base edges を事前抽出
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // 枝刈り 0: 新頂点 x の次数 + remaining < 3 なら skip
        int deg_x = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) {
                deg_x += (int)(tmp & 1);
                tmp >>= 1;
            }
        }
        if (deg_x + remaining < 3) continue;

        // Set edges from x to {1,...,k} based on mask
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) {
                state.deg[u]++;
            }
        }
        state.deg[x] = deg_x;
        state.alive_count = x;

        bool prune = false;

        // 枝刈り 1: 平面性 (遺伝的)
        {
            std::vector<std::pair<int, int>> edges = base_edges;
            for (int u = 1; u <= k; ++u)
                if (state.adj[x][u])
                    edges.push_back(std::make_pair(u, x));
            Graph g(x, edges);
            PlanarResult pr = check_planar(g);
            if (!pr.is_planar) {
                prune = true;
            }
        }

        // 枝刈り 2: 連結性
        if (!prune) {
            int comp = polyhedral_count_components(state, x);
            if (comp > remaining + 1) {
                prune = true;
            }
        }

        // 枝刈り 3: 次数下限 (3-連結には全頂点 deg >= 3 必要)
        if (!prune && remaining <= 2) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 3) {
                    prune = true;
                    break;
                }
            }
        }

        if (!prune) {
            polyhedral_enum_dfs(state, out);
        }

        // Restore
        for (int u = 1; u <= k; ++u) {
            if (state.adj[x][u]) {
                state.deg[u]--;
            }
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.deg[x] = 0;
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き多面体グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return PolyhedralEnumerationResult
 *
 * 逆探索 (reverse search) を使用。3-連結は遺伝的でないため、
 * 中間ステップでは平面性 + 連結性 + 次数の枝刈りを行い、
 * 最終ステップで完全な 3-連結 + 平面判定を行う。
 */
inline PolyhedralEnumerationResult
enumerate_polyhedral_graphs(int n,
    PolyhedralEnumAlgorithm algo =
        PolyhedralEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    PolyhedralEnumerationResult result;
    if (n < 4) return result;
    detail::PolyhedralEnumState root(n);
    detail::polyhedral_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
