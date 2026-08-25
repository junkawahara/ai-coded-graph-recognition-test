#ifndef GRAPH_RECOGNITION_WEAKLY_CHORDAL_H
#define GRAPH_RECOGNITION_WEAKLY_CHORDAL_H

/**
 * @file weakly_chordal.h
 * @brief Weakly chordal graph recognition
 *
 * Algorithm: detect holes (induced cycles >= 5) in G and in its complement.
 *   - CO_CHORDAL_BIPARTITE: explicitly builds the complement graph
 *     (the enum name is historical; no chordal-bipartite reduction is used)
 *   - COMPLEMENT_BFS: detects anti-holes via complement BFS without
 *     materializing the complement (default)
 *   Both are polynomial but well above O(n*m): candidate hole edges are
 *   enumerated pairwise and each candidate pair triggers a BFS
 *   (already Theta(n^3) on edgeless graphs).
 */

#include "graph.h"
#include "graph_utils.h"
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for weakly chordal graph recognition
 */
enum class WeaklyChordalAlgorithm {
    CO_CHORDAL_BIPARTITE, /**< Explicit complement construction + hole detection (name is historical) */
    COMPLEMENT_BFS        /**< Hole detection with complement BFS, no explicit complement (default) */
};

/**
 * @brief Result of weakly chordal graph recognition
 */
struct WeaklyChordalResult {
    bool is_weakly_chordal = false; /**< true if the graph is weakly chordal */
};

namespace detail_weakly_chordal {

/** @brief Determines whether an induced cycle of length >= 5 exists (internal) */
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

                    std::vector<int> bfs;
                    bfs.reserve(n);
                    seen[x] = seen_token;
                    dist[x] = 0;
                    bfs.push_back(x);

                    for (size_t qi = 0; qi < bfs.size() && seen[y] != seen_token; ++qi) {
                        int cur = bfs[qi];
                        for (size_t in = 0; in < g.adj[cur].size(); ++in) {
                            int nxt = g.adj[cur][in];
                            if (seen[nxt] == seen_token) continue;
                            if (blocked_stamp[nxt] == blocked_token &&
                                nxt != x && nxt != y) {
                                continue;
                            }
                            seen[nxt] = seen_token;
                            dist[nxt] = dist[cur] + 1;
                            bfs.push_back(nxt);
                        }
                    }

                    // If dist(x,y) >= 2, then u-x-...-y-v-u is a hole of length >= 5.
                    if (seen[y] == seen_token && dist[y] >= 2) return true;
                }
            }
        }
    }

    return false;
}

/**
 * @brief Detect induced cycles of length >= 5 in the complement graph without building it
 *
 * Executes the same logic as has_induced_cycle_ge5 on the complement graph.
 * Complement graph edges = non-edges of G, complement adjacency = non-adjacency in G.
 */
inline bool has_anti_hole_ge5(const Graph& g) {
    int n = g.n;
    if (n < 5) return false;

    // Complement graph degree (non-adjacency count)
    std::vector<int> comp_deg(n + 1, 0);
    for (int u = 1; u <= n; ++u) {
        comp_deg[u] = n - 1 - (int)g.adj[u].size();
    }

    std::vector<int> blocked_stamp(n + 1, 0);
    std::vector<int> seen(n + 1, 0), dist(n + 1, 0);
    int blocked_token = 0, seen_token = 0;

    // Pre-allocate BFS data structures outside the inner loops
    std::vector<int> rem_prev(n + 2), rem_next(n + 2);
    std::vector<unsigned char> g_adj_stamp(n + 2, 0);
    std::vector<int> to_remove;
    to_remove.reserve(n);

    for (int u = 1; u <= n; ++u) {
        if (comp_deg[u] < 2) continue;

        for (int v = u + 1; v <= n; ++v) {
            if (g.has_edge(u, v)) continue; // Edge in G -> non-edge in complement
            if (comp_deg[v] < 2) continue;

            // Complement graph edge (u,v)
            // blocked = N_comp(u) ∪ N_comp(v) ∪ {u,v}
            if (blocked_token == INT_MAX) {
                std::fill(blocked_stamp.begin(), blocked_stamp.end(), 0);
                blocked_token = 0;
            }
            blocked_token++;
            blocked_stamp[u] = blocked_token;
            blocked_stamp[v] = blocked_token;
            // N_comp(u): w != u, !has_edge(u,w)
            for (int w = 1; w <= n; ++w) {
                if (w != u && !g.has_edge(u, w)) {
                    blocked_stamp[w] = blocked_token;
                }
            }
            // N_comp(v): w != v, !has_edge(v,w)
            for (int w = 1; w <= n; ++w) {
                if (w != v && !g.has_edge(v, w)) {
                    blocked_stamp[w] = blocked_token;
                }
            }

            // x ∈ N_comp(u), x != v, !comp_edge(x,v) i.e. has_edge(x,v)
            for (int x = 1; x <= n; ++x) {
                if (x == u || x == v) continue;
                if (g.has_edge(u, x)) continue; // x not in N_comp(u)
                if (!g.has_edge(x, v)) continue; // x in N_comp(v) → skip

                // y ∈ N_comp(v), y != u, !comp_edge(y,u) i.e. has_edge(y,u)
                for (int y = 1; y <= n; ++y) {
                    if (y == u || y == v || y == x) continue;
                    if (g.has_edge(v, y)) continue; // y not in N_comp(v)
                    if (!g.has_edge(y, u)) continue; // y in N_comp(u) → skip

                    // BFS in complement from x to y, avoiding blocked (except x,y).
                    // Uses complement BFS technique with a remaining-set linked list
                    // for O(n + m_complement) per BFS instead of O(n^2).
                    if (seen_token == INT_MAX) {
                        std::fill(seen.begin(), seen.end(), 0);
                        seen_token = 0;
                    }
                    seen_token++;

                    // Initialize doubly-linked list of remaining (unvisited) vertices.
                    // Sentinels: 0 is head, n+1 is tail.
                    {
                        int prev_node = 0;
                        for (int w = 1; w <= n; ++w) {
                            // Skip blocked vertices (except x and y)
                            if (blocked_stamp[w] == blocked_token
                                && w != x && w != y) continue;
                            // Skip source x (will be seen immediately)
                            if (w == x) continue;
                            rem_prev[w] = prev_node;
                            rem_next[prev_node] = w;
                            prev_node = w;
                        }
                        rem_next[prev_node] = n + 1;
                        rem_prev[n + 1] = prev_node;
                    }

                    std::vector<int> bfs;
                    bfs.reserve(n);
                    seen[x] = seen_token;
                    dist[x] = 0;
                    bfs.push_back(x);

                    for (size_t qi = 0; qi < bfs.size() && seen[y] != seen_token; ++qi) {
                        int cur = bfs[qi];

                        // Step 1: stamp all G-neighbors of cur
                        for (size_t gi = 0; gi < g.adj[cur].size(); ++gi) {
                            g_adj_stamp[g.adj[cur][gi]] = 1;
                        }

                        // Step 2: iterate remaining set; complement-neighbors
                        // are those NOT stamped (not adjacent in G)
                        to_remove.clear();
                        for (int w = rem_next[0]; w != n + 1; w = rem_next[w]) {
                            if (!g_adj_stamp[w]) {
                                // w is a complement-neighbor of cur
                                seen[w] = seen_token;
                                dist[w] = dist[cur] + 1;
                                bfs.push_back(w);
                                to_remove.push_back(w);
                            }
                        }

                        // Step 3: remove visited vertices from remaining set
                        for (size_t ri = 0; ri < to_remove.size(); ++ri) {
                            int w = to_remove[ri];
                            rem_next[rem_prev[w]] = rem_next[w];
                            rem_prev[rem_next[w]] = rem_prev[w];
                        }

                        // Step 4: clear stamps
                        for (size_t gi = 0; gi < g.adj[cur].size(); ++gi) {
                            g_adj_stamp[g.adj[cur][gi]] = 0;
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

/** @brief Complement construction + induced cycle detection (original algorithm) */
inline WeaklyChordalResult check_weakly_chordal_co(const Graph& g) {
    WeaklyChordalResult res;
    res.is_weakly_chordal = false;

    if (detail_weakly_chordal::has_induced_cycle_ge5(g)) return res;

    Graph gc = build_complement(g);
    if (detail_weakly_chordal::has_induced_cycle_ge5(gc)) return res;

    res.is_weakly_chordal = true;
    return res;
}

/**
 * @brief BFS avoiding complement construction
 *
 * G's holes are detected directly; anti-holes are detected via complement BFS.
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
 * @brief Determines whether the graph is weakly chordal
 * @param g Input graph
 * @param algo Algorithm to use (default: COMPLEMENT_BFS)
 * @return WeaklyChordalResult
 *
 * Weakly chordal <=> neither G nor complement(G) contains an induced
 * cycle of length >= 5.
 */
inline WeaklyChordalResult check_weakly_chordal(const Graph& g,
    WeaklyChordalAlgorithm algo = WeaklyChordalAlgorithm::COMPLEMENT_BFS) {
    switch (algo) {
        case WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE:
            return check_weakly_chordal_co(g);
        case WeaklyChordalAlgorithm::COMPLEMENT_BFS:
            return check_weakly_chordal_complement_bfs(g);
        default:
            break;
    }
    return WeaklyChordalResult();
}

} // namespace graph_recognition

#endif
