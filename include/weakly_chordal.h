#ifndef GRAPH_RECOGNITION_WEAKLY_CHORDAL_H
#define GRAPH_RECOGNITION_WEAKLY_CHORDAL_H

/**
 * @file weakly_chordal.h
 * @brief 弱弦グラフ (weakly chordal graph) 認識
 *
 * グラフと補グラフの両方に、長さ 5 以上の誘導閉路 (hole) が
 * 存在しないことを検査する。
 */

#include "graph.h"
#include <climits>
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

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

} // namespace detail_weakly_chordal

/**
 * @brief グラフが弱弦グラフか判定する
 * @param g 入力グラフ
 * @return WeaklyChordalResult
 *
 * weakly chordal ⟺ G と complement(G) のいずれにも長さ 5 以上の
 * 誘導閉路が存在しない。
 */
inline WeaklyChordalResult check_weakly_chordal(const Graph& g) {
    WeaklyChordalResult res;
    res.is_weakly_chordal = false;

    if (detail_weakly_chordal::has_induced_cycle_ge5(g)) return res;

    Graph gc = detail_weakly_chordal::build_complement_graph(g);
    if (detail_weakly_chordal::has_induced_cycle_ge5(gc)) return res;

    res.is_weakly_chordal = true;
    return res;
}

} // namespace graph_recognition

#endif
