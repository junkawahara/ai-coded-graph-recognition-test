#ifndef GRAPH_RECOGNITION_EVEN_HOLE_FREE_H
#define GRAPH_RECOGNITION_EVEN_HOLE_FREE_H

/**
 * @file even_hole_free.h
 * @brief Even-hole-free グラフ認識
 *
 * Even-hole-free グラフとは、長さ 4 以上の偶数長誘導閉路 (even hole) を
 * 含まないグラフである。最小の偶数穴は C4。
 *
 * アルゴリズム:
 *   perfect.h の has_odd_hole() と同様の手法を偶奇反転して適用。
 *   各辺 (u,v) について、x ∈ N(u)\N[v], y ∈ N(v)\N[u] の組を調べ、
 *   G \ (N[u] ∪ N[v] \ {x,y}) における x-y 間の奇数長誘導パスを探索。
 *   奇数長パスが存在すれば u-x-path-y-v-u が偶数穴。
 *
 * 参考文献:
 *   - Conforti, Cornuejols, Kapoor, Vuskovic, JCTB, 2002
 *   - Lai, Lu, Thorup, STOC 2020
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Even-hole-free グラフ認識の結果
 */
struct EvenHoleFreeResult {
    bool is_even_hole_free = false; /**< even-hole-free であれば true */
};

namespace detail_even_hole_free {

/**
 * @brief DFS で奇数長の誘導パスを探索 (偶数穴検出用)
 *
 * blocked 以外の頂点を使い、start から target へ奇数長 (>=1) の
 * 誘導パス (弦なしパス) が存在するか判定する。
 */
inline bool dfs_odd_path(const Graph& g,
                          std::vector<int>& path,
                          std::vector<bool>& in_path,
                          const std::vector<bool>& blocked,
                          int target) {
    int cur = path.back();
    int edges = (int)path.size() - 1;

    for (size_t j = 0; j < g.adj[cur].size(); ++j) {
        int w = g.adj[cur][j];
        if (blocked[w] && w != target) continue;
        if (in_path[w]) continue;

        // 誘導パス条件: w は path[0..edges-1] と非隣接
        bool ok = true;
        for (int i = 0; i <= edges - 1; ++i) {
            if (g.has_edge(w, path[i])) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        if (w == target) {
            if ((edges + 1) >= 1 && (edges + 1) % 2 == 1) {
                return true;
            }
            continue;
        }

        path.push_back(w);
        in_path[w] = true;
        if (dfs_odd_path(g, path, in_path, blocked, target)) return true;
        path.pop_back();
        in_path[w] = false;
    }

    return false;
}

/**
 * @brief G に偶数穴 (長さ>=4 の偶数長誘導閉路) が存在するか判定
 *
 * 各辺 (u,v) について制限グラフ上の BFS + DFS で検出。
 */
inline bool has_even_hole(const Graph& g) {
    int n = g.n;
    if (n < 4) return false;

    std::vector<bool> blocked(n + 1, false);
    std::vector<int> dist(n + 1, -1);
    std::vector<bool> in_path(n + 1, false);
    std::vector<int> path;

    for (int u = 1; u <= n; ++u) {
        if (g.adj[u].size() < 2) continue;

        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (u > v) continue;
            if (g.adj[v].size() < 2) continue;

            // N[u] ∪ N[v] をブロック
            std::vector<int> blocked_list;
            blocked[u] = true; blocked_list.push_back(u);
            blocked[v] = true; blocked_list.push_back(v);
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                if (!blocked[g.adj[u][i]]) {
                    blocked[g.adj[u][i]] = true;
                    blocked_list.push_back(g.adj[u][i]);
                }
            }
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                if (!blocked[g.adj[v][i]]) {
                    blocked[g.adj[v][i]] = true;
                    blocked_list.push_back(g.adj[v][i]);
                }
            }

            // x ∈ N(u) \ N[v], y ∈ N(v) \ N[u]
            for (size_t xi = 0; xi < g.adj[u].size(); ++xi) {
                int x = g.adj[u][xi];
                if (x == v) continue;
                if (g.has_edge(x, v)) continue;

                for (size_t yi = 0; yi < g.adj[v].size(); ++yi) {
                    int y = g.adj[v][yi];
                    if (y == u || y == x) continue;
                    if (g.has_edge(y, u)) continue;

                    // x, y のブロック解除
                    blocked[x] = false;
                    blocked[y] = false;

                    // BFS from x in restricted graph
                    std::queue<int> q;
                    std::vector<int> visited;
                    dist[x] = 0;
                    visited.push_back(x);
                    q.push(x);
                    bool is_bipartite = true;

                    while (!q.empty()) {
                        int cur = q.front();
                        q.pop();
                        for (size_t ni = 0; ni < g.adj[cur].size(); ++ni) {
                            int nxt = g.adj[cur][ni];
                            if (blocked[nxt]) continue;
                            if (dist[nxt] >= 0) {
                                if ((dist[nxt] % 2) == (dist[cur] % 2)) {
                                    is_bipartite = false;
                                }
                                continue;
                            }
                            dist[nxt] = dist[cur] + 1;
                            visited.push_back(nxt);
                            q.push(nxt);
                        }
                    }

                    bool found = false;
                    if (dist[y] >= 1) {
                        if (dist[y] % 2 == 1) {
                            // 奇数長最短パス → 偶数穴
                            found = true;
                        } else if (!is_bipartite) {
                            // 偶数長最短、非二部 → DFS で奇数長誘導パス探索
                            path.clear();
                            path.push_back(x);
                            in_path[x] = true;
                            found = dfs_odd_path(g, path, in_path,
                                                  blocked, y);
                            for (size_t i = 0; i < path.size(); ++i) {
                                in_path[path[i]] = false;
                            }
                        }
                    }

                    // BFS クリーンアップ
                    for (size_t i = 0; i < visited.size(); ++i) {
                        dist[visited[i]] = -1;
                    }

                    blocked[x] = true;
                    blocked[y] = true;

                    if (found) {
                        for (size_t i = 0; i < blocked_list.size(); ++i) {
                            blocked[blocked_list[i]] = false;
                        }
                        return true;
                    }
                }
            }

            // ブロック解除
            for (size_t i = 0; i < blocked_list.size(); ++i) {
                blocked[blocked_list[i]] = false;
            }
        }
    }

    return false;
}

} // namespace detail_even_hole_free

/**
 * @brief グラフが even-hole-free か判定する
 * @param g 入力グラフ
 * @return EvenHoleFreeResult
 */
inline EvenHoleFreeResult check_even_hole_free(const Graph& g) {
    EvenHoleFreeResult res;
    if (g.n <= 3) {
        res.is_even_hole_free = true;
        return res;
    }
    res.is_even_hole_free = !detail_even_hole_free::has_even_hole(g);
    return res;
}

} // namespace graph_recognition

#endif
