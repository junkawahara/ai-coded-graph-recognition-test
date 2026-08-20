#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_H

/**
 * @file chordal_bipartite.h
 * @brief Chordal bipartite graph recognition
 *
 * Algorithms:
 *   - CYCLE_CHECK: check for induced even cycles of length >= 6
 *   - BISIMPLICIAL: bisimplicial edge elimination (brute force) O(m*n^4)
 *   - FAST_BISIMPLICIAL: bisimplicial edge elimination (using adjacency lists) O(m^2*Delta^2) (default)
 *
 * Memory: BISIMPLICIAL and FAST_BISIMPLICIAL allocate an n x n adjacency
 * matrix, i.e. Theta(n^2) bytes even for sparse graphs (~1 GB at n = 32768).
 * Use CYCLE_CHECK (adjacency-list based) when n is large and memory matters.
 */

#include "bipartite.h"
#include "graph.h"
#include <climits>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for chordal bipartite graph recognition
 */
enum class ChordalBipartiteAlgorithm {
    CYCLE_CHECK,       /**< check for induced even cycles of length >= 6 */
    BISIMPLICIAL,      /**< bisimplicial edge elimination (brute force) O(m*n^4) */
    FAST_BISIMPLICIAL  /**< bisimplicial edge elimination (using adjacency lists) O(m^2*Delta^2) (default) */
};

/**
 * @brief Result of chordal bipartite graph recognition
 */
struct ChordalBipartiteResult {
    bool is_chordal_bipartite = false;  /**< true if the graph is chordal bipartite */
    std::vector<int> color;     /**< bipartite coloring (valid only when is_chordal_bipartite == true) */
};

namespace detail {

/** @brief Determines whether an induced even cycle of length >= 6 exists (internal function) */
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
                if (g.has_edge(x, v)) continue; // L5: safety check for non-bipartite input
                for (size_t iy = 0; iy < g.adj[v].size(); ++iy) {
                    int y = g.adj[v][iy];
                    if (y == u || y == x) continue; // L4: added y == x check
                    if (g.has_edge(y, u)) continue; // L5: safety check for non-bipartite input

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
 * @brief Chordal bipartite graph recognition via induced even cycle check
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
 * @brief Chordal bipartite graph recognition via bisimplicial edge elimination O(m*n^4)
 *
 * Removes one edge at a time. Each step scans all vertex pairs O(n^2),
 * and each candidate edge's bisimplicial test takes O(n^2). Over m steps: O(m*n^4).
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
 * @brief Bisimplicial edge elimination (using adjacency lists) O(m^2*Delta^2)
 *
 * Improves bisimplicial test to O(deg(u)*deg(v)) using adjacency matrix + dynamic adjacency lists.
 * Removes one edge at a time; each step scans all edges O(m) to find a bisimplicial edge.
 * Each edge test: O(Delta^2); over m steps: O(m^2*Delta^2).
 */
inline ChordalBipartiteResult check_chordal_bipartite_fast_bisimplicial(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;

    // Adjacency matrix
    std::vector<std::vector<unsigned char>> adj(
        n + 1, std::vector<unsigned char>(n + 1, 0));
    // Dynamic adjacency lists
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
                if (u > v) continue; // Process each edge only once

                // Bisimplicial check: N(u) x N(v) is complete bipartite
                // Assume u is on color-0 side, v is on color-1 side
                // Verify adj[a][b] for each a in N(v)\{u} and each b in N(u)\{v}
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
                    // Remove edge (u, v)
                    adj[u][v] = 0;
                    adj[v][u] = 0;
                    edge_count--;
                    // Remove from adjacency lists
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
 * @brief Determines whether a graph is a chordal bipartite graph
 * @param g Input graph
 * @param algo Algorithm to use (default: FAST_BISIMPLICIAL)
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
        default:
            break;
    }
    return ChordalBipartiteResult();
}

} // namespace graph_recognition

#endif
