#ifndef GRAPH_RECOGNITION_BICONNECTED_ENUM_H
#define GRAPH_RECOGNITION_BICONNECTED_ENUM_H

/**
 * @file biconnected_enum.h
 * @brief 2-連結 (biconnected) グラフの列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き 2-連結グラフを全列挙する。
 *
 * parent(G) = G から最大ラベルの頂点を除去
 *
 * 2-連結は遺伝的 (hereditary) でないため、中間ステップでの
 * 性質ベース枝刈りは行わない。代わりに以下の枝刈りを適用:
 *   1. 連結性枝刈り: 連結成分数 > 残り頂点数 + 1 なら枝刈り
 *   2. 次数下限枝刈り: 最終2レベルで各頂点の次数制約をチェック
 *   3. 最終ステップでの完全 2-連結判定
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "biconnected.h"
#include "chordal_enum.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief 2-連結列挙アルゴリズムの選択
 */
enum class BiconnectedEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 */
};

/**
 * @brief 2-連結列挙の結果
 */
struct BiconnectedEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された 2-連結グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct BiconnectedEnumState {
    int total_n;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    std::vector<int> deg;  /**< 各頂点の次数 */

    explicit BiconnectedEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief {1, ..., x} 上の連結成分数を数える
 */
inline int count_components(const BiconnectedEnumState& state, int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        // BFS
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
 * @brief 2-連結逆探索の DFS
 *
 * 頂点 alive_count+1 を追加し、{1,...,alive_count} の部分集合を
 * 近傍として試す。非遺伝的クラスのため性質ベース枝刈りは行わず、
 * 連結性ベースの枝刈りと最終ステップでの完全判定を行う。
 */
inline void biconnected_enum_dfs(BiconnectedEnumState& state,
                                  std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // 最終ステップ: 完全な 2-連結判定
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    edges.push_back(std::make_pair(u, v));

        Graph g(state.total_n, edges);
        BiconnectedResult res = check_biconnected(g);
        if (res.is_biconnected) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            graph.edges = edges;
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    // 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // Set edges from x to {1,...,k} based on mask
        int deg_x = 0;
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) {
                state.deg[u]++;
                deg_x++;
            }
        }
        state.deg[x] = deg_x;
        state.alive_count = x;

        bool prune = false;

        // 枝刈り 1: 連結性
        // 連結成分数 c のとき、残り remaining 頂点で最大 remaining 個の
        // 成分を統合可能なので c > remaining + 1 なら連結不可能
        int comp = count_components(state, x);
        if (comp > remaining + 1) {
            prune = true;
        }

        // 枝刈り 2: 次数下限
        // 最終グラフで全頂点 deg >= 2 が必要。
        // 頂点 v (v <= x) は残り remaining 頂点から最大 remaining 辺を獲得可能。
        // deg[v] + remaining < 2 なら不可能。
        if (!prune && remaining <= 1) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 2) {
                    prune = false;
                    prune = true;
                    break;
                }
            }
        }

        if (!prune) {
            biconnected_enum_dfs(state, out);
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
 * @brief 頂点集合 {1, ..., n} 上のラベル付き 2-連結グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return BiconnectedEnumerationResult
 *
 * 逆探索 (reverse search) を使用。2-連結は遺伝的でないため、
 * 中間ステップでは連結性ベースの枝刈りのみ行い、
 * 最終ステップで完全な 2-連結判定を行う。
 */
inline BiconnectedEnumerationResult
enumerate_biconnected_graphs(int n,
    BiconnectedEnumAlgorithm algo =
        BiconnectedEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BiconnectedEnumerationResult result;
    if (n < 3) return result;
    detail::BiconnectedEnumState root(n);
    detail::biconnected_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
