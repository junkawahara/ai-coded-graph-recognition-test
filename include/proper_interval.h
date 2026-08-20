#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_H

/**
 * @file proper_interval.h
 * @brief Proper interval graph recognition
 *
 * Algorithm:
 *   - TRIPLE_LOOP_CLAW_CHECK: Triple-loop claw detection O(n*Delta^3)
 *   - FAST_CLAW_CHECK: Edge-counting claw detection O(m*Delta) (default)
 */

#include "graph.h"
#include "interval.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for proper interval graph recognition
 */
enum class ProperIntervalAlgorithm {
    TRIPLE_LOOP_CLAW_CHECK, /**< Triple-loop claw detection O(n*Delta^3) */
    FAST_CLAW_CHECK /**< Edge-counting claw detection O(m*Delta)~O(n*Delta^3) (default) */
};

/**
 * @brief Result of proper interval graph recognition
 */
struct ProperIntervalResult {
    bool is_proper_interval = false; /**< true if the graph is a proper interval graph */
};

namespace detail {

/**
 * @brief Determines whether an induced claw (K_{1,3}) exists (original algorithm)
 */
inline bool has_induced_claw_triple(const Graph& g) {
    for (int c = 1; c <= g.n; ++c) {
        if (g.adj_set[c].size() < 3) continue;

        std::vector<int> nbrs;
        nbrs.reserve(g.adj_set[c].size());
        for (std::unordered_set<int>::const_iterator it = g.adj_set[c].begin();
             it != g.adj_set[c].end(); ++it) {
            nbrs.push_back(*it);
        }

        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Claw detection via edge counting
 *
 * Complexity: edge-counting phase is O(m*Delta). Claw search in non-complete
 * neighborhoods is O(n*Delta^3) worst case, but vertices with complete
 * neighborhoods are skipped in O(1), so practical performance on sparse graphs approaches O(m*Delta).
 *
 * For each vertex c, count edges within N(c).
 * If d = deg(c) and edge count == d(d-1)/2, then N(c) is complete -> no claw.
 * Otherwise, N(c) has non-edges, so search for a claw.
 */
inline bool has_induced_claw_fast(const Graph& g) {
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

        // N(c) has non-edges -> search for claw
        // Stamp N(c) (again)
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        bool found_claw = false;
        // Find non-edge (a, b)
        for (size_t i = 0; i < g.adj[c].size() && !found_claw; ++i) {
            int a = g.adj[c][i];
            // Mark neighbors of a within N(c)
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                if (stamped[g.adj[a][j]]) a_adj[g.adj[a][j]] = 1;
            }

            for (size_t j = i + 1; j < g.adj[c].size() && !found_claw; ++j) {
                int b = g.adj[c][j];
                if (a_adj[b]) continue; // a-b is an edge

                // Non-edge (a, b) found. Find x in N(c) with x != a, b, !edge(x,a), !edge(x,b)
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

        if (found_claw) return true;
    }
    return false;
}

/** @brief TRIPLE_LOOP_CLAW_CHECK: original algorithm (interval + naive claw search) */
inline ProperIntervalResult check_proper_interval_triple_loop(const Graph& g) {
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (has_induced_claw_triple(g)) return res;

    res.is_proper_interval = true;
    return res;
}

/** @brief FAST_CLAW_CHECK: fast check via edge counting */
inline ProperIntervalResult check_proper_interval_fast(const Graph& g) {
    ProperIntervalResult res;
    res.is_proper_interval = false;

    IntervalResult interval = check_interval(g);
    if (!interval.is_interval) return res;

    if (has_induced_claw_fast(g)) return res;

    res.is_proper_interval = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a proper interval graph
 * @param g Input graph
 * @param algo Algorithm to use (default: FAST_CLAW_CHECK)
 * @return ProperIntervalResult
 *
 * G is a proper interval graph <=> G is an interval graph and claw-free.
 */
inline ProperIntervalResult check_proper_interval(const Graph& g,
    ProperIntervalAlgorithm algo = ProperIntervalAlgorithm::FAST_CLAW_CHECK) {
    switch (algo) {
        case ProperIntervalAlgorithm::TRIPLE_LOOP_CLAW_CHECK:
            return detail::check_proper_interval_triple_loop(g);
        case ProperIntervalAlgorithm::FAST_CLAW_CHECK:
            return detail::check_proper_interval_fast(g);
        default:
            break;
    }
    return ProperIntervalResult();
}

} // namespace graph_recognition

#endif
