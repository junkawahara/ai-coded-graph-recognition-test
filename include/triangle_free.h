#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_H

/**
 * @file triangle_free.h
 * @brief Triangle-free graph (K3-free graph) recognition
 *
 * A triangle-free graph is a graph that does not contain a triangle (K3)
 * as an induced subgraph. That is, no triple of mutually adjacent vertices exists.
 *
 * Algorithm:
 *   - BRUTE: Enumerate all 3-vertex combinations and search for K3 O(n^3)
 *   - EDGE_PAIR: Check for common neighbors per edge (u,v) O(m*Delta)
 *               (default)
 *
 * References:
 *   - Folklore; small forbidden subgraph detection techniques
 *   - Itai, Rodeh, "Finding a minimum circuit in a graph,"
 *     SIAM J. Comput. 7(4), 1978
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for triangle-free graph recognition
 */
enum class TriangleFreeAlgorithm {
    BRUTE,     /**< Enumerate all 3-vertex combinations O(n^3) */
    EDGE_PAIR  /**< Common neighbor check per edge O(m*Delta) (default) */
};

/**
 * @brief Result of triangle-free graph recognition
 */
struct TriangleFreeResult {
    bool is_triangle_free = false; /**< true if the graph is triangle-free */
};

namespace detail {

/**
 * @brief Detect triangles by enumerating all 3-vertex combinations
 *
 * Checks if all pairs are adjacent for every 3-vertex combination.
 * Complexity: O(n^3)
 */
inline TriangleFreeResult check_triangle_free_brute(const Graph& g) {
    TriangleFreeResult res;
    res.is_triangle_free = true;

    for (int a = 1; a <= g.n; ++a) {
        for (int b = a + 1; b <= g.n; ++b) {
            if (!g.has_edge(a, b)) continue;
            for (int c = b + 1; c <= g.n; ++c) {
                if (g.has_edge(a, c) && g.has_edge(b, c)) {
                    res.is_triangle_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief Triangle detection via common neighbor check per edge O(m*Delta)
 *
 * For each edge (u,v), checks whether any neighbor w of u is also adjacent to v.
 * If even one common neighbor is found, a triangle exists.
 *
 * Complexity: O(m * Delta) where Delta is the maximum degree.
 */
inline TriangleFreeResult check_triangle_free_edge_pair(const Graph& g) {
    TriangleFreeResult res;
    res.is_triangle_free = true;

    int n = g.n;
    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; // Process each edge only once

            // Check if any neighbor of u is also adjacent to v
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (w != v && g.has_edge(w, v)) {
                    res.is_triangle_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is triangle-free (K3-free)
 * @param g Input graph
 * @param algo Algorithm to use (default: EDGE_PAIR)
 * @return TriangleFreeResult
 *
 * G is triangle-free <=> G contains no K3 as an induced subgraph.
 * Equivalent condition: no common neighbor of u and v exists for any edge (u,v).
 */
inline TriangleFreeResult check_triangle_free(const Graph& g,
    TriangleFreeAlgorithm algo = TriangleFreeAlgorithm::EDGE_PAIR) {
    switch (algo) {
        case TriangleFreeAlgorithm::BRUTE:
            return detail::check_triangle_free_brute(g);
        case TriangleFreeAlgorithm::EDGE_PAIR:
            return detail::check_triangle_free_edge_pair(g);
        default:
            break;
    }
    return TriangleFreeResult();
}

} // namespace graph_recognition

#endif
