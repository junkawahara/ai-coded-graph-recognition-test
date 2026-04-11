#ifndef GRAPH_RECOGNITION_KTREE_ENUM_H
#define GRAPH_RECOGNITION_KTREE_ENUM_H

/**
 * @file ktree_enum.h
 * @brief k-木 (k-tree) の列挙 (逆探索)
 *
 * 逆探索 (reverse search) により頂点集合 {1, ..., n} 上の
 * ラベル付き k-tree を全列挙する。
 *
 * k-tree は帰納的に定義される:
 *   - K_{k+1} は k-tree
 *   - k-tree G の k-クリークに隣接する新頂点を追加しても k-tree
 *
 * k=1: 木、k=2: maximal outerplanar (n>=3)、k=3: Apollonian network。
 * chordal graph の部分クラスで treewidth がちょうど k。
 *
 * 逆探索の親関数: 最大ラベルの「次数 <= k のシンプリシャル頂点」を除去。
 * 子の生成: 新頂点を k-クリークに隣接させ、正準性チェック。
 *
 * 参考文献:
 *   Beineke, Pippert, "The number of labeled k-dimensional trees,"
 *   J. Combin. Theory 6(2), 1969
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief k-tree 列挙の結果
 */
struct KTreeEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された k-tree の配列 */
};

namespace detail {

/**
 * @brief 頂点 v がシンプリシャルかつ次数 <= k か判定
 */
inline bool ktree_is_simplicial_leq_k(const ChordalEnumState& state,
                                       int v, int k) {
    std::vector<int> neighbors;
    for (int u = 1; u <= state.total_n; ++u) {
        if (state.alive[u] && state.adj[v][u]) neighbors.push_back(u);
    }
    if ((int)neighbors.size() > k) return false;
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        for (std::size_t j = i + 1; j < neighbors.size(); ++j) {
            if (!state.adj[neighbors[i]][neighbors[j]]) return false;
        }
    }
    return true;
}

/**
 * @brief k-tree 逆探索の正準除去頂点
 *
 * 最大ラベルの「シンプリシャルかつ次数 <= k」の頂点を返す。
 */
inline int ktree_canonical_removed_vertex(const ChordalEnumState& state,
                                           int k) {
    int best = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (!state.alive[v]) continue;
        if (ktree_is_simplicial_leq_k(state, v, k)) best = v;
    }
    return best;
}

/**
 * @brief サイズちょうど k のクリークを列挙する DFS
 */
inline void enumerate_exact_k_cliques_dfs(const ChordalEnumState& state,
                                           const std::vector<int>& vertices,
                                           std::size_t start_idx,
                                           int target_size,
                                           std::vector<int>* current,
                                           std::vector<std::vector<int>>* out) {
    if ((int)current->size() == target_size) {
        out->push_back(*current);
        return;
    }
    int remaining_needed = target_size - (int)current->size();
    if ((int)(vertices.size() - start_idx) < remaining_needed) return;

    for (std::size_t i = start_idx; i < vertices.size(); ++i) {
        int v = vertices[i];
        bool ok = true;
        for (std::size_t j = 0; j < current->size(); ++j) {
            if (!state.adj[v][(*current)[j]]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;
        current->push_back(v);
        enumerate_exact_k_cliques_dfs(state, vertices, i + 1,
                                       target_size, current, out);
        current->pop_back();
    }
}

/**
 * @brief alive 頂点中のサイズちょうど k のクリークを全列挙
 */
inline std::vector<std::vector<int>> enumerate_exact_k_cliques(
    const ChordalEnumState& state, int k) {
    std::vector<int> vertices;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) vertices.push_back(v);
    }
    std::vector<std::vector<int>> cliques;
    if (k == 0) {
        cliques.push_back(std::vector<int>());
        return cliques;
    }
    std::vector<int> current;
    enumerate_exact_k_cliques_dfs(state, vertices, 0, k, &current, &cliques);
    return cliques;
}

/**
 * @brief k-tree 逆探索の in-place DFS
 *
 * alive_count < k の場合: K_{k+1} 構築中。新頂点を全 alive 頂点に隣接。
 * alive_count >= k の場合: k-クリークを列挙し新頂点を各 k-クリークに隣接。
 */
inline void ktree_reverse_search_dfs(ChordalEnumState& state, int k,
                                      std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<int> missing;
    for (int x = 1; x <= state.total_n; ++x) {
        if (!state.alive[x]) missing.push_back(x);
    }

    std::vector<std::vector<int>> cliques;
    if (state.alive_count < k) {
        // K_{k+1} 構築中: 新頂点を全 alive 頂点に隣接させる
        std::vector<int> all_alive;
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.alive[v]) all_alive.push_back(v);
        }
        cliques.push_back(all_alive);
    } else {
        // k-クリークを列挙
        cliques = enumerate_exact_k_cliques(state, k);
    }

    for (std::size_t i = 0; i < missing.size(); ++i) {
        int x = missing[i];
        for (std::size_t j = 0; j < cliques.size(); ++j) {
            const std::vector<int>& clique = cliques[j];

            // x を in-place で追加
            state.alive[x] = 1;
            ++state.alive_count;
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 1;
                state.adj[clique[ci]][x] = 1;
            }

            // 正準性チェック
            int best = ktree_canonical_removed_vertex(state, k);
            if (best == x) {
                ktree_reverse_search_dfs(state, k, out);
            }

            // 復元
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 0;
                state.adj[clique[ci]][x] = 0;
            }
            state.alive[x] = 0;
            --state.alive_count;
        }
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き k-tree を全列挙する
 * @param n 頂点数
 * @param k パラメータ (treewidth)
 * @return KTreeEnumerationResult
 *
 * n < k+1 の場合は空結果 (k-tree は K_{k+1} が最小)。
 * k=0 の場合は 0 辺の空グラフ 1 個。
 */
inline KTreeEnumerationResult enumerate_ktree_graphs_reverse_search(int n,
                                                                     int k) {
    KTreeEnumerationResult result;
    if (n <= 0 || k < 0) return result;
    if (n < k + 1) return result;
    detail::ChordalEnumState root(n);
    detail::ktree_reverse_search_dfs(root, k, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
