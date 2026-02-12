#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H

/**
 * @file chordal_bipartite.h
 * @brief 弦二部グラフ (chordal bipartite graph) 認識
 *
 * アルゴリズム:
 *   - CYCLE_CHECK: 長さ 6 以上の誘導偶閉路の存在検査
 *   - BISIMPLICIAL: bisimplicial 辺消去 (全探索)
 *   - FAST_BISIMPLICIAL: bisimplicial 辺消去 (隣接リスト使用) (デフォルト)
 */

#include "bipartite.h"
#include "graph.h"
#include <climits>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 弦二部グラフ認識アルゴリズムの選択
 */
enum class ChordalBipartiteAlgorithm {
    CYCLE_CHECK,       /**< 長さ 6 以上の誘導偶閉路の存在検査 */
    BISIMPLICIAL,      /**< bisimplicial 辺消去 (全探索) */
    FAST_BISIMPLICIAL  /**< bisimplicial 辺消去 (隣接リスト使用) (デフォルト) */
};

/**
 * @brief 弦二部グラフ認識の結果
 */
struct ChordalBipartiteResult {
    bool is_chordal_bipartite;  /**< 弦二部グラフであれば true */
    std::vector<int> color;     /**< 二部彩色 (is_chordal_bipartite == true の場合のみ有効) */
};

namespace detail {

/** @brief 長さ 6 以上の誘導偶閉路が存在するか判定する (内部関数) */
inline bool has_induced_even_cycle_ge6(
    const Graph& g,
    const std::vector<int>& color) {
    int n = g.n;
    std::vector<int> blocked_stamp(n + 1, 0);
    std::vector<int> seen(n + 1, 0), dist(n + 1, 0);
    int blocked_token = 0, seen_token = 0;

    for (int u = 1; u <= n; ++u) {
        if (color[u] != 0) continue;
        if (g.adj[u].size() < 2) continue;

        for (size_t iv = 0; iv < g.adj[u].size(); ++iv) {
            int v = g.adj[u][iv];
            if (g.adj[v].size() < 2) continue;

            if (blocked_token == INT_MAX) {
                std::fill(blocked_stamp.begin(), blocked_stamp.end(), 0);
                blocked_token = 0;
            }
            blocked_token++;
            blocked_stamp[u] = blocked_token;
            blocked_stamp[v] = blocked_token;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                blocked_stamp[g.adj[u][i]] = blocked_token;
            }
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                blocked_stamp[g.adj[v][i]] = blocked_token;
            }

            for (size_t ix = 0; ix < g.adj[u].size(); ++ix) {
                int x = g.adj[u][ix];
                if (x == v) continue;
                for (size_t iy = 0; iy < g.adj[v].size(); ++iy) {
                    int y = g.adj[v][iy];
                    if (y == u) continue;

                    if (seen_token == INT_MAX) {
                        std::fill(seen.begin(), seen.end(), 0);
                        seen_token = 0;
                    }
                    seen_token++;

                    std::queue<int> q;
                    seen[x] = seen_token;
                    dist[x] = 0;
                    q.push(x);

                    while (!q.empty() && seen[y] != seen_token) {
                        int cur = q.front();
                        q.pop();
                        for (size_t in = 0; in < g.adj[cur].size(); ++in) {
                            int nxt = g.adj[cur][in];
                            if (seen[nxt] == seen_token) continue;
                            if (blocked_stamp[nxt] == blocked_token &&
                                nxt != x && nxt != y) {
                                continue;
                            }
                            seen[nxt] = seen_token;
                            dist[nxt] = dist[cur] + 1;
                            q.push(nxt);
                        }
                    }

                    if (seen[y] == seen_token && dist[y] >= 3) return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief 誘導偶閉路検査による弦二部グラフ認識
 */
inline ChordalBipartiteResult check_chordal_bipartite_cycle_check(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    if (has_induced_even_cycle_ge6(g, bip.color)) return res;

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

/**
 * @brief bisimplicial 辺消去による弦二部グラフ認識
 */
inline ChordalBipartiteResult check_chordal_bipartite_bisimplicial(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;

    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    int edge_count = 0;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) {
                adj[u][v] = 1;
                adj[v][u] = 1;
                edge_count++;
            }
        }
    }

    while (edge_count > 0) {
        bool found = false;

        for (int u = 1; u <= n && !found; ++u) {
            for (int v = u + 1; v <= n && !found; ++v) {
                if (!adj[u][v]) continue;
                bool bisimplicial = true;
                for (int a = 1; a <= n && bisimplicial; ++a) {
                    if (!adj[v][a]) continue;
                    for (int b = 1; b <= n && bisimplicial; ++b) {
                        if (!adj[u][b]) continue;
                        if (!adj[a][b]) bisimplicial = false;
                    }
                }

                if (bisimplicial) {
                    found = true;
                    adj[u][v] = 0;
                    adj[v][u] = 0;
                    edge_count--;
                }
            }
        }

        if (!found) return res;
    }

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

/**
 * @brief bisimplicial 辺消去 (隣接リスト使用)
 *
 * 隣接行列 + 動的隣接リストで bisimplicial 判定を O(deg(u)·deg(v)) に改善。
 * 辺除去は 1 本ずつ行い、各ステップで全辺を走査して bisimplicial 辺を探す。
 */
inline ChordalBipartiteResult check_chordal_bipartite_fast_bisimplicial(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;

    // 隣接行列
    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    // 動的隣接リスト
    std::vector<std::vector<int>> nbrs(n + 1);
    int edge_count = 0;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) {
                adj[u][v] = 1;
                adj[v][u] = 1;
                edge_count++;
            }
        }
        nbrs[u] = g.adj[u];
    }

    while (edge_count > 0) {
        bool found = false;

        for (int u = 1; u <= n && !found; ++u) {
            for (size_t ei = 0; ei < nbrs[u].size() && !found; ++ei) {
                int v = nbrs[u][ei];
                if (u > v) continue; // 各辺を 1 回だけ処理

                // bisimplicial チェック: N(u)×N(v) が完全二部
                // u は色 0 側、v は色 1 側とする
                // N(v)\{u} の各 a と N(u)\{v} の各 b について adj[a][b] を検証
                bool bisimplicial = true;
                for (size_t ai = 0; ai < nbrs[v].size() && bisimplicial; ++ai) {
                    int a = nbrs[v][ai];
                    if (a == u) continue;
                    for (size_t bi = 0; bi < nbrs[u].size() && bisimplicial; ++bi) {
                        int b = nbrs[u][bi];
                        if (b == v) continue;
                        if (!adj[a][b]) bisimplicial = false;
                    }
                }

                if (bisimplicial) {
                    found = true;
                    // 辺 (u, v) を除去
                    adj[u][v] = 0;
                    adj[v][u] = 0;
                    edge_count--;
                    // 隣接リストから除去
                    for (size_t i = 0; i < nbrs[u].size(); ++i) {
                        if (nbrs[u][i] == v) {
                            nbrs[u][i] = nbrs[u].back();
                            nbrs[u].pop_back();
                            break;
                        }
                    }
                    for (size_t i = 0; i < nbrs[v].size(); ++i) {
                        if (nbrs[v][i] == u) {
                            nbrs[v][i] = nbrs[v].back();
                            nbrs[v].pop_back();
                            break;
                        }
                    }
                }
            }
        }

        if (!found) return res;
    }

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

} // namespace detail

/**
 * @brief グラフが弦二部グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: FAST_BISIMPLICIAL)
 * @return ChordalBipartiteResult
 */
inline ChordalBipartiteResult check_chordal_bipartite(const Graph& g,
    ChordalBipartiteAlgorithm algo = ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL) {
    switch (algo) {
        case ChordalBipartiteAlgorithm::CYCLE_CHECK:
            return detail::check_chordal_bipartite_cycle_check(g);
        case ChordalBipartiteAlgorithm::BISIMPLICIAL:
            return detail::check_chordal_bipartite_bisimplicial(g);
        case ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL:
            return detail::check_chordal_bipartite_fast_bisimplicial(g);
    }
    return ChordalBipartiteResult();
}

} // namespace graph_recognition

#endif
