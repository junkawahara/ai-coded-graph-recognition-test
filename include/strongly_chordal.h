#ifndef GRAPH_RECOGNITION_STRONGLY_CHORDAL_H
#define GRAPH_RECOGNITION_STRONGLY_CHORDAL_H

/**
 * @file strongly_chordal.h
 * @brief 強弦グラフ (strongly chordal graph) 認識
 *
 * アルゴリズム:
 *   - STRONG_ELIMINATION: 全スキャン simple vertex 除去 O(n⁴)
 *   - PEO_MATRIX: 隣接行列 + PEO 順序処理 O(n² + m·Δ) (デフォルト)
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief 強弦グラフ認識アルゴリズムの選択
 */
enum class StronglyChordalAlgorithm {
    STRONG_ELIMINATION, /**< 全スキャン simple vertex 除去 O(n⁴) */
    PEO_MATRIX          /**< 隣接行列 + PEO 順序処理 O(n² + m·Δ) (デフォルト) */
};

/**
 * @brief 強弦グラフ認識の結果
 */
struct StronglyChordalResult {
    bool is_strongly_chordal; /**< 強弦グラフであれば true */
};

namespace detail_strongly_chordal {

/** @brief alive 部分グラフにおける v の隣接頂点を列挙する */
inline void collect_alive_neighbors(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    neighbors->clear();
    for (size_t i = 0; i < g.adj[v].size(); ++i) {
        int u = g.adj[v][i];
        if (alive[u]) neighbors->push_back(u);
    }
}

/** @brief alive 部分グラフで N[x] が N[y] に含まれるか判定する */
inline bool is_closed_neighborhood_subset(
    const Graph& g,
    int x,
    int y,
    const std::vector<unsigned char>& alive) {
    // x in N[x] が N[y] に入る必要があるので、x==y か xy が辺。
    if (x != y && !g.has_edge(x, y)) return false;

    for (size_t i = 0; i < g.adj[x].size(); ++i) {
        int z = g.adj[x][i];
        if (!alive[z]) continue;
        if (z == y) continue;
        if (!g.has_edge(y, z)) return false;
    }
    return true;
}

/** @brief alive 部分グラフで v が simple vertex か判定する */
inline bool is_simple_vertex(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    collect_alive_neighbors(g, v, alive, neighbors);

    // simple vertex は simplicial である必要がある。
    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (!g.has_edge(x, y)) return false;
        }
    }

    // 近傍の閉近傍同士が包含で比較可能。
    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (is_closed_neighborhood_subset(g, x, y, alive)) continue;
            if (is_closed_neighborhood_subset(g, y, x, alive)) continue;
            return false;
        }
    }

    return true;
}

} // namespace detail_strongly_chordal

/** @brief 全スキャン simple vertex 除去 (元のアルゴリズム) */
inline StronglyChordalResult check_strongly_chordal_elimination(const Graph& g) {
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    std::vector<unsigned char> alive(g.n + 1, 1);
    std::vector<int> neighbors;
    neighbors.reserve(g.n);

    int remaining = g.n;
    while (remaining > 0) {
        int pick = 0;
        for (int v = 1; v <= g.n; ++v) {
            if (!alive[v]) continue;
            if (detail_strongly_chordal::is_simple_vertex(g, v, alive, &neighbors)) {
                pick = v;
                break;
            }
        }
        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
    }

    res.is_strongly_chordal = true;
    return res;
}

/**
 * @brief 隣接行列 + 全スキャン simple vertex 除去 O(n² + n·m·Δ)
 *
 * 1. 弦グラフチェック (bucket MCS + PEO 検証): O(n+m)
 * 2. 隣接行列構築: O(n²)
 * 3. 全スキャンで simple vertex を探索・除去 (行列で O(1) 辺判定)
 */
inline StronglyChordalResult check_strongly_chordal_peo_matrix(const Graph& g) {
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    int n = g.n;
    if (n <= 1) { res.is_strongly_chordal = true; return res; }

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    // 隣接行列構築
    std::vector<std::vector<unsigned char>> adj_mat(n + 1, std::vector<unsigned char>(n + 1, 0));
    for (int u = 1; u <= n; ++u) {
        for (size_t j = 0; j < g.adj[u].size(); ++j) {
            adj_mat[u][g.adj[u][j]] = 1;
        }
    }

    std::vector<unsigned char> alive(n + 1, 1);
    std::vector<int> alive_deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        alive_deg[v] = (int)g.adj[v].size();
    }

    int remaining = n;
    std::vector<int> nbrs;
    nbrs.reserve(n);

    while (remaining > 0) {
        int pick = 0;
        for (int v = 1; v <= n && pick == 0; ++v) {
            if (!alive[v]) continue;

            // alive な近傍を収集
            nbrs.clear();
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (alive[u]) nbrs.push_back(u);
            }

            // simplicial チェック (行列で O(1))
            bool simplicial = true;
            for (size_t a = 0; a < nbrs.size() && simplicial; ++a) {
                for (size_t b = a + 1; b < nbrs.size() && simplicial; ++b) {
                    if (!adj_mat[nbrs[a]][nbrs[b]]) simplicial = false;
                }
            }
            if (!simplicial) continue;

            // simple チェック: nbrs を alive_deg 昇順ソート
            std::sort(nbrs.begin(), nbrs.end(), [&](int a, int b) {
                return alive_deg[a] < alive_deg[b];
            });

            bool simple = true;
            for (size_t j = 0; j + 1 < nbrs.size() && simple; ++j) {
                int x = nbrs[j], y = nbrs[j + 1];
                for (size_t k = 0; k < g.adj[x].size(); ++k) {
                    int u = g.adj[x][k];
                    if (!alive[u]) continue;
                    if (u == y) continue;
                    if (!adj_mat[y][u]) { simple = false; break; }
                }
            }
            if (!simple) continue;

            pick = v;
        }

        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
        for (size_t j = 0; j < g.adj[pick].size(); ++j) {
            int u = g.adj[pick][j];
            if (alive[u]) alive_deg[u]--;
        }
    }

    res.is_strongly_chordal = true;
    return res;
}

/**
 * @brief グラフが強弦グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: PEO_MATRIX)
 * @return StronglyChordalResult
 */
inline StronglyChordalResult check_strongly_chordal(const Graph& g,
    StronglyChordalAlgorithm algo = StronglyChordalAlgorithm::PEO_MATRIX) {
    switch (algo) {
        case StronglyChordalAlgorithm::STRONG_ELIMINATION:
            return check_strongly_chordal_elimination(g);
        case StronglyChordalAlgorithm::PEO_MATRIX:
            return check_strongly_chordal_peo_matrix(g);
    }
    return StronglyChordalResult();
}

} // namespace graph_recognition

#endif
