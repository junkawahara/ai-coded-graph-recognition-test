#ifndef GRAPH_RECOGNITION_WEAKLY_CHORDAL_H
#define GRAPH_RECOGNITION_WEAKLY_CHORDAL_H

/**
 * @file weakly_chordal.h
 * @brief 弱弦グラフ (weakly chordal graph) 認識
 *
 * アルゴリズム:
 *   - CO_CHORDAL_BIPARTITE: 補グラフ構築 + 誘導閉路検査 O(n² + n·m)
 *   - COMPLEMENT_BFS: 補グラフ構築を回避した BFS O(n·m) (デフォルト)
 */

#include "graph.h"
#include <climits>
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 弱弦グラフ認識アルゴリズムの選択
 */
enum class WeaklyChordalAlgorithm {
    CO_CHORDAL_BIPARTITE, /**< 補グラフ構築 + 誘導閉路検査 O(n² + n·m) */
    COMPLEMENT_BFS        /**< 補グラフ構築を回避した BFS O(n·m) (デフォルト) */
};

/**
 * @brief 弱弦グラフ認識の結果
 */
struct WeaklyChordalResult {
    bool is_weakly_chordal; /**< 弱弦グラフであれば true */
};

namespace detail_weakly_chordal {

/** @brief 補グラフを構築する (内部関数) */
inline Graph build_complement_graph(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

/** @brief 長さ 5 以上の誘導閉路が存在するか判定する (内部関数) */
inline bool has_induced_cycle_ge5(const Graph& g) {
    int n = g.n;
    if (n < 5) return false;

    std::vector<int> blocked_stamp(n + 1, 0);
    std::vector<int> seen(n + 1, 0), dist(n + 1, 0);
    int blocked_token = 0, seen_token = 0;

    for (int u = 1; u <= n; ++u) {
        if (g.adj[u].size() < 2) continue;

        for (size_t iv = 0; iv < g.adj[u].size(); ++iv) {
            int v = g.adj[u][iv];
            if (u > v) continue;
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
                if (g.has_edge(x, v)) continue;

                for (size_t iy = 0; iy < g.adj[v].size(); ++iy) {
                    int y = g.adj[v][iy];
                    if (y == u || y == x) continue;
                    if (g.has_edge(y, u)) continue;

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

                    // dist(x,y) >= 2 なら u-x-...-y-v-u が長さ >= 5 の hole。
                    if (seen[y] == seen_token && dist[y] >= 2) return true;
                }
            }
        }
    }

    return false;
}

/**
 * @brief 補グラフ上の長さ 5 以上の誘導閉路を、補グラフ構築なしで検出
 *
 * has_induced_cycle_ge5 と同じロジックを補グラフ上で実行。
 * 補グラフの辺 = G の非辺、補グラフの隣接 = G の非隣接。
 */
inline bool has_anti_hole_ge5(const Graph& g) {
    int n = g.n;
    if (n < 5) return false;

    // 隣接行列を構築 (小さいグラフなので OK)
    std::vector<std::vector<unsigned char>> adj_mat(n + 1,
        std::vector<unsigned char>(n + 1, 0));
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            adj_mat[u][g.adj[u][i]] = 1;
        }
    }

    // 補グラフの次数 (非隣接数)
    std::vector<int> comp_deg(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        comp_deg[u] = n - 1 - (int)g.adj[u].size();
    }

    std::vector<int> blocked_stamp(n + 1, 0);
    std::vector<int> seen(n + 1, 0), dist(n + 1, 0);
    int blocked_token = 0, seen_token = 0;

    for (int u = 1; u <= n; ++u) {
        if (comp_deg[u] < 2) continue;

        for (int v = u + 1; v <= n; ++v) {
            if (adj_mat[u][v]) continue; // G の辺 → 補グラフの非辺
            if (comp_deg[v] < 2) continue;

            // 補グラフ辺 (u,v)
            // blocked = N_comp(u) ∪ N_comp(v) ∪ {u,v}
            if (blocked_token == INT_MAX) {
                std::fill(blocked_stamp.begin(), blocked_stamp.end(), 0);
                blocked_token = 0;
            }
            blocked_token++;
            blocked_stamp[u] = blocked_token;
            blocked_stamp[v] = blocked_token;
            // N_comp(u): w != u, !adj_mat[u][w]
            for (int w = 1; w <= n; ++w) {
                if (w != u && !adj_mat[u][w]) {
                    blocked_stamp[w] = blocked_token;
                }
            }
            // N_comp(v): w != v, !adj_mat[v][w]
            for (int w = 1; w <= n; ++w) {
                if (w != v && !adj_mat[v][w]) {
                    blocked_stamp[w] = blocked_token;
                }
            }

            // x ∈ N_comp(u), x != v, !comp_edge(x,v) i.e. adj_mat[x][v]==1
            for (int x = 1; x <= n; ++x) {
                if (x == u || x == v) continue;
                if (adj_mat[u][x]) continue; // x not in N_comp(u)
                if (!adj_mat[x][v]) continue; // x in N_comp(v) → skip

                // y ∈ N_comp(v), y != u, !comp_edge(y,u) i.e. adj_mat[y][u]==1
                for (int y = 1; y <= n; ++y) {
                    if (y == u || y == v || y == x) continue;
                    if (adj_mat[v][y]) continue; // y not in N_comp(v)
                    if (!adj_mat[y][u]) continue; // y in N_comp(u) → skip

                    // BFS in complement from x to y, avoiding blocked (except x,y)
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
                        // 補グラフの隣接: w != cur, !adj_mat[cur][w]
                        for (int w = 1; w <= n; ++w) {
                            if (w == cur) continue;
                            if (seen[w] == seen_token) continue;
                            if (blocked_stamp[w] == blocked_token
                                && w != x && w != y) continue;
                            if (adj_mat[cur][w]) continue; // G の辺 → 補グラフの非辺
                            seen[w] = seen_token;
                            dist[w] = dist[cur] + 1;
                            q.push(w);
                        }
                    }

                    if (seen[y] == seen_token && dist[y] >= 2) return true;
                }
            }
        }
    }
    return false;
}

} // namespace detail_weakly_chordal

/** @brief 補グラフ構築 + 誘導閉路検査 (元のアルゴリズム) */
inline WeaklyChordalResult check_weakly_chordal_co(const Graph& g) {
    WeaklyChordalResult res;
    res.is_weakly_chordal = false;

    if (detail_weakly_chordal::has_induced_cycle_ge5(g)) return res;

    Graph gc = detail_weakly_chordal::build_complement_graph(g);
    if (detail_weakly_chordal::has_induced_cycle_ge5(gc)) return res;

    res.is_weakly_chordal = true;
    return res;
}

/**
 * @brief 補グラフ構築を回避した BFS
 *
 * G の hole は直接検出、anti-hole は補グラフ BFS で検出。
 */
inline WeaklyChordalResult check_weakly_chordal_complement_bfs(const Graph& g) {
    WeaklyChordalResult res;
    res.is_weakly_chordal = false;

    if (detail_weakly_chordal::has_induced_cycle_ge5(g)) return res;
    if (detail_weakly_chordal::has_anti_hole_ge5(g)) return res;

    res.is_weakly_chordal = true;
    return res;
}

/**
 * @brief グラフが弱弦グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: COMPLEMENT_BFS)
 * @return WeaklyChordalResult
 *
 * weakly chordal ⟺ G と complement(G) のいずれにも長さ 5 以上の
 * 誘導閉路が存在しない。
 */
inline WeaklyChordalResult check_weakly_chordal(const Graph& g,
    WeaklyChordalAlgorithm algo = WeaklyChordalAlgorithm::COMPLEMENT_BFS) {
    switch (algo) {
        case WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE:
            return check_weakly_chordal_co(g);
        case WeaklyChordalAlgorithm::COMPLEMENT_BFS:
            return check_weakly_chordal_complement_bfs(g);
    }
    return WeaklyChordalResult();
}

} // namespace graph_recognition

#endif
