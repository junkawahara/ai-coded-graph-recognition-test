#ifndef GRAPH_RECOGNITION_PERFECT_H
#define GRAPH_RECOGNITION_PERFECT_H

/**
 * @file perfect.h
 * @brief 完全グラフ (perfect graph) 認識
 *
 * Strong Perfect Graph Theorem (Chudnovsky-Robertson-Seymour-Thomas 2006):
 *   G が完全グラフ ⟺ G に奇数穴 (odd hole, 長さ>=5) も
 *   奇数反穴 (odd antihole, 長さ>=5) も存在しない。
 *
 * アルゴリズム:
 *   各辺 (u,v) について、x ∈ N(u)\N[v], y ∈ N(v)\N[u] の組を調べ、
 *   G \ (N[u] ∪ N[v] \ {x,y}) における x-y 間の偶数長誘導パスを探索。
 *   偶数長パスが存在すれば u-x-path-y-v-u が奇数穴。
 *   BFS で最短パスの偶奇を判定し、非二部の場合は DFS で探索。
 *   補グラフ上で同様の処理を行い奇数反穴を検出。
 */

#include "graph.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 完全グラフ認識の結果
 */
struct PerfectResult {
    bool is_perfect = false; /**< 完全グラフであれば true */
};

namespace detail_perfect {

/** @brief 補グラフを構築する */
inline Graph build_complement(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (!g.has_edge(u, v)) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return Graph(g.n, edges);
}

/**
 * @brief DFS で偶数長の誘導パスを探索
 *
 * blocked 以外の頂点を使い、start から target へ偶数長 (>=2) の
 * 誘導パス (弦なしパス) が存在するか判定する。
 */
inline bool dfs_even_path(const Graph& g,
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
            if ((edges + 1) >= 2 && (edges + 1) % 2 == 0) {
                return true;
            }
            continue; // 奇数長では target を中間頂点にしない
        }

        path.push_back(w);
        in_path[w] = true;
        if (dfs_even_path(g, path, in_path, blocked, target)) return true;
        path.pop_back();
        in_path[w] = false;
    }

    return false;
}

/**
 * @brief G に奇数穴 (長さ>=5 の誘導奇数閉路) が存在するか判定
 *
 * 各辺 (u,v) について制限グラフ上の BFS + DFS で検出。
 */
inline bool has_odd_hole(const Graph& g) {
    int n = g.n;
    if (n < 5) return false;

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
                    if (dist[y] >= 2) {
                        if (dist[y] % 2 == 0) {
                            // 偶数長最短パス → 奇数穴
                            found = true;
                        } else if (!is_bipartite) {
                            // 奇数長最短、非二部 → DFS で偶数長誘導パス探索
                            path.clear();
                            path.push_back(x);
                            in_path[x] = true;
                            found = dfs_even_path(g, path, in_path,
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

} // namespace detail_perfect

/**
 * @brief グラフが完全グラフか判定する
 * @param g 入力グラフ
 * @return PerfectResult
 *
 * Strong Perfect Graph Theorem に基づき、奇数穴と奇数反穴の
 * 非存在を確認する。
 */
inline PerfectResult check_perfect(const Graph& g) {
    PerfectResult res;
    res.is_perfect = true;

    if (g.n <= 4) return res;

    // 奇数穴の検出
    if (detail_perfect::has_odd_hole(g)) {
        res.is_perfect = false;
        return res;
    }

    // 奇数反穴の検出 (補グラフの奇数穴)
    Graph gc = detail_perfect::build_complement(g);
    if (detail_perfect::has_odd_hole(gc)) {
        res.is_perfect = false;
        return res;
    }

    return res;
}

} // namespace graph_recognition

#endif
