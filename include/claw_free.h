#ifndef GRAPH_RECOGNITION_CLAW_FREE_H
#define GRAPH_RECOGNITION_CLAW_FREE_H

/**
 * @file claw_free.h
 * @brief Claw-free graph (K_{1,3}-free graph) recognition
 *
 * A claw-free graph is a graph that does not contain K_{1,3} (claw) as an induced subgraph.
 * K_{1,3} consists of a center vertex c and 3 vertices a, b, d adjacent to c
 * where a, b, d are mutually non-adjacent.
 *
 * It is a superclass of line graphs, and proper interval graphs are also claw-free.
 *
 * Algorithms:
 *   - TRIPLE_LOOP: search for independent sets of 3 vertices in each vertex's neighborhood O(n*Delta^3)
 *   - EDGE_COUNT: determine if neighborhood is complete by edge counting; search if not O(m*Delta) (default)
 *
 * References:
 *   - Minty (1980); Sbihi (1980); Chudnovsky & Seymour (2005)
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for claw-free graph recognition
 */
enum class ClawFreeAlgorithm {
    TRIPLE_LOOP, /**< triple-loop claw detection O(n*Delta^3) */
    EDGE_COUNT   /**< edge-counting claw detection O(m*Delta) (default) */
};

/**
 * @brief Result of claw-free graph recognition
 */
struct ClawFreeResult {
    bool is_claw_free = false; /**< true if the graph is claw-free */
};

namespace detail {

/**
 * @brief Induced claw (K_{1,3}) detection via triple loop
 *
 * For each vertex c, checks whether there exist 3 mutually non-adjacent vertices (a, b, d) in N(c).
 * Complexity: O(n*Delta^3) where Delta is the maximum degree.
 */
inline ClawFreeResult check_claw_free_triple(const Graph& g) {
    ClawFreeResult res;
    res.is_claw_free = true;

    for (int c = 1; c <= g.n; ++c) {
        if (g.adj[c].size() < 3) continue;

        const std::vector<int>& nbrs = g.adj[c];

        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    res.is_claw_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief Fast claw detection via edge counting O(m*Delta)
 *
 * Counts the number of edges within N(c) for each vertex c.
 * If edge count == d(d-1)/2 where d = deg(c), then N(c) is complete -> no claw.
 * Otherwise, N(c) has a non-edge, so search for a claw.
 */
inline ClawFreeResult check_claw_free_edge_count(const Graph& g) {
    ClawFreeResult res;
    res.is_claw_free = true;

    int n = g.n;
    std::vector<unsigned char> stamped(n + 1, 0);
    std::vector<unsigned char> a_adj(n + 1, 0);

    for (int c = 1; c <= n; ++c) {
        int d = (int)g.adj[c].size();
        if (d < 3) continue;

        // Stamp N(c)
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        // Count edges within N(c)
        long long edge_count = 0;
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            int u = g.adj[c][i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (stamped[w] && w > u) edge_count++;
            }
        }

        // Clear stamps
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        long long need = (long long)d * (d - 1) / 2;
        if (edge_count == need) continue; // N(c) is a complete graph

        // N(c) has a non-edge -> search for a claw
        // Stamp N(c) (again)
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        bool found_claw = false;
        // Find non-edge (a, b) and search for x not adjacent to either a or b
        for (size_t i = 0; i < g.adj[c].size() && !found_claw; ++i) {
            int a = g.adj[c][i];
            // Mark a's neighbors within N(c)
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                if (stamped[g.adj[a][j]]) a_adj[g.adj[a][j]] = 1;
            }

            for (size_t j = i + 1; j < g.adj[c].size() && !found_claw; ++j) {
                int b = g.adj[c][j];
                if (a_adj[b]) continue; // a-b is an edge

                // Non-edge (a, b) found. x in N(c) with x != a, b, !edge(x,a), !edge(x,b)
                for (size_t k = 0; k < g.adj[c].size(); ++k) {
                    int x = g.adj[c][k];
                    if (x == a || x == b) continue;
                    if (!a_adj[x] && !g.has_edge(x, b)) {
                        found_claw = true;
                        break;
                    }
                }
            }

            // Clear a_adj
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                a_adj[g.adj[a][j]] = 0;
            }
        }

        // Clear stamps
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        if (found_claw) {
            res.is_claw_free = false;
            return res;
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is claw-free (K_{1,3}-free)
 * @param g Input graph
 * @param algo Algorithm to use (default: EDGE_COUNT)
 * @return ClawFreeResult
 *
 * G is claw-free iff it does not contain K_{1,3} as an induced subgraph.
 */
inline ClawFreeResult check_claw_free(const Graph& g,
    ClawFreeAlgorithm algo = ClawFreeAlgorithm::EDGE_COUNT) {
    switch (algo) {
        case ClawFreeAlgorithm::TRIPLE_LOOP:
            return detail::check_claw_free_triple(g);
        case ClawFreeAlgorithm::EDGE_COUNT:
            return detail::check_claw_free_edge_count(g);
        default:
            break;
    }
    return ClawFreeResult();
}

} // namespace graph_recognition

#endif
