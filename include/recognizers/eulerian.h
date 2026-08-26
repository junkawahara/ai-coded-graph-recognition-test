#ifndef GRAPH_RECOGNITION_EULERIAN_H
#define GRAPH_RECOGNITION_EULERIAN_H

/**
 * @file eulerian.h
 * @brief Eulerian graph recognition
 *
 * An Eulerian graph is a graph where all vertices have even degree.
 * A connected Eulerian graph has an Euler circuit.
 *
 * Algorithms:
 *   - DEGREE_CHECK: Check parity of each vertex degree O(n)
 *
 * References:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 */

#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Eulerian graph recognition
 */
enum class EulerianAlgorithm {
    DEGREE_CHECK /**< Even degree check O(n) */
};

/**
 * @brief Result of Eulerian graph recognition
 */
struct EulerianResult {
    bool is_eulerian = false; /**< true if all vertices have even degree */
};

namespace detail {

/**
 * @brief Checks whether each vertex has even degree
 *
 * Complexity: O(n)
 */
inline EulerianResult check_eulerian_degree(const Graph& g) {
    EulerianResult res;
    for (int v = 1; v <= g.n; ++v) {
        if (g.adj[v].size() % 2 != 0) {
            return res;
        }
    }
    res.is_eulerian = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is Eulerian (all vertices have even degree)
 * @param g Input graph
 * @param algo Algorithm to use (default: DEGREE_CHECK)
 * @return EulerianResult
 *
 * G is Eulerian iff deg(v) is even for all vertices v.
 * If connected, an Euler circuit exists.
 */
inline EulerianResult check_eulerian(const Graph& g,
    EulerianAlgorithm algo = EulerianAlgorithm::DEGREE_CHECK) {
    switch (algo) {
        case EulerianAlgorithm::DEGREE_CHECK:
            return detail::check_eulerian_degree(g);
        default:
            break;
    }
    return EulerianResult();
}

} // namespace graph_recognition

#endif
