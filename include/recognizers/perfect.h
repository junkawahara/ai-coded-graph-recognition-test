#ifndef GRAPH_RECOGNITION_PERFECT_H
#define GRAPH_RECOGNITION_PERFECT_H

/**
 * @file perfect.h
 * @brief Perfect graph recognition
 *
 * Strong Perfect Graph Theorem (Chudnovsky-Robertson-Seymour-Thomas 2006):
 *   G is a perfect graph iff G contains neither odd holes (length >= 5)
 *   nor odd antiholes (length >= 5).
 *
 * Algorithms:
 *   For each edge (u,v), examines pairs x in N(u) \\ N[v], y in N(v) \\ N[u],
 *   and searches for even-length induced x-y paths in G \ (N[u] ∪ N[v] \ {x,y}).
 *   If an even-length path exists, u-x-path-y-v-u forms an odd hole.
 *   Determines the parity of shortest paths via BFS, and searches via DFS for non-bipartite cases.
 *   Performs the same process on the complement graph to detect odd antiholes.
 *
 * Complexity / practical limits:
 *   The antihole check builds the complement explicitly, so even a sparse
 *   input incurs an odd-hole search on a graph with Theta(n^2) edges; the
 *   induced even-path DFS is exponential in the worst case (the bipartite
 *   BFS shortcut prunes most instances). Measured: a sparse chordal graph
 *   with n = 400 takes ~1.7 s, K(200,200) ~27 s, K(400,400) over a minute.
 *   A few hundred vertices is the practical limit. For sparse graphs
 *   check_odd_hole_free() is far cheaper because it skips the complement.
 *   The Chudnovsky-Scott-Seymour-Spirkl polynomial-time odd-hole detection
 *   (JACM 67(1), 2020) is not implemented here.
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include "certificates/obstruction_extract.h"
#include <queue>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Result of perfect graph recognition
 */
struct PerfectResult {
    bool is_perfect = false; /**< true if the graph is perfect */
    Obstruction obstruction; /**< NO certificate: an ODD_HOLE, in g or -- with
                                  in_complement set -- in its complement, where it is
                                  the odd antihole. Valid only when is_perfect == false */
};

namespace detail_perfect {

/**
 * @brief DFS search for even-length induced paths
 *
 * Using only non-blocked vertices, determines whether an even-length (>=2)
 * induced path (chordless path) exists from start to target.
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

        // Induced path condition: w is non-adjacent to path[0..edges-1]
        bool ok = true;
        for (int i = 0; i <= edges - 1; ++i) {
            if (g.has_edge(w, path[i])) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;

        if (w == target) {
            if ((edges + 1) % 2 == 0) {
                return true;
            }
            continue; // Do not use target as an intermediate vertex for odd-length paths
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
 * @brief Determines whether G contains an odd hole (induced odd cycle of length >= 5)
 *
 * Detected via BFS + DFS on restricted graphs for each edge (u,v).
 */
inline std::vector<int> find_odd_hole(const Graph& g) {
    int n = g.n;
    if (n < 5) return std::vector<int>();

    std::vector<bool> blocked(n + 1, false);
    std::vector<int> dist(n + 1, -1);
    std::vector<int> par(n + 1, 0);
    std::vector<bool> in_path(n + 1, false);
    std::vector<int> path;
    std::vector<int> hole;

    for (int u = 1; u <= n; ++u) {
        if (g.adj[u].size() < 2) continue;

        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (u > v) continue;
            if (g.adj[v].size() < 2) continue;

            // Block N[u] ∪ N[v]
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

                    // Unblock x, y
                    blocked[x] = false;
                    blocked[y] = false;

                    // BFS from x in restricted graph
                    std::queue<int> q;
                    std::vector<int> visited;
                    dist[x] = 0;
                    par[x] = 0;
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
                            par[nxt] = cur;
                            visited.push_back(nxt);
                            q.push(nxt);
                        }
                    }

                    bool found = false;
                    if (dist[y] >= 2) {
                        if (dist[y] % 2 == 0) {
                            // Even-length shortest path -> odd hole. The path
                            // avoided N[u] and N[v] and is shortest, so closing
                            // it through the edge uv is already chordless.
                            found = true;
                            hole = detail_obstruction::hole_from_bfs_path(u, v, x, y, par);
                        } else if (!is_bipartite) {
                            // Odd-length shortest path, non-bipartite -> DFS search for even-length induced path
                            path.clear();
                            path.push_back(x);
                            in_path[x] = true;
                            found = dfs_even_path(g, path, in_path,
                                                   blocked, y);
                            for (size_t i = 0; i < path.size(); ++i) {
                                in_path[path[i]] = false;
                            }
                            if (found) {
                                // dfs_even_path stops before pushing the target,
                                // so y is appended here.
                                hole.clear();
                                hole.push_back(u);
                                for (size_t i = 0; i < path.size(); ++i) {
                                    hole.push_back(path[i]);
                                }
                                hole.push_back(y);
                                hole.push_back(v);
                            }
                        }
                    }

                    // BFS cleanup
                    for (size_t i = 0; i < visited.size(); ++i) {
                        dist[visited[i]] = -1;
                    }

                    blocked[x] = true;
                    blocked[y] = true;

                    if (found) {
                        for (size_t i = 0; i < blocked_list.size(); ++i) {
                            blocked[blocked_list[i]] = false;
                        }
                        return hole;
                    }
                }
            }

            // Unblock
            for (size_t i = 0; i < blocked_list.size(); ++i) {
                blocked[blocked_list[i]] = false;
            }
        }
    }

    return std::vector<int>();
}

/** @brief Determines whether G contains an odd hole */
inline bool has_odd_hole(const Graph& g) {
    return !find_odd_hole(g).empty();
}

} // namespace detail_perfect

/**
 * @brief Determines whether the graph is a perfect graph
 * @param g Input graph
 * @return PerfectResult
 *
 * Based on the Strong Perfect Graph Theorem, verifies the absence
 * of odd holes and odd antiholes.
 */
inline PerfectResult check_perfect(const Graph& g) {
    PerfectResult res;
    res.is_perfect = true;

    if (g.n <= 4) return res;

    // Odd hole detection
    std::vector<int> hole = detail_perfect::find_odd_hole(g);
    if (!hole.empty()) {
        res.is_perfect = false;
        res.obstruction =
            detail_obstruction::cycle_obstruction(hole, ObstructionKind::ODD_HOLE);
        return res;
    }

    // Odd antihole detection (odd holes in the complement graph)
    Graph gc = build_complement(g);
    std::vector<int> anti = detail_perfect::find_odd_hole(gc);
    if (!anti.empty()) {
        res.is_perfect = false;
        res.obstruction =
            detail_obstruction::cycle_obstruction(anti, ObstructionKind::ODD_HOLE, true);
        return res;
    }

    return res;
}

} // namespace graph_recognition

#endif
