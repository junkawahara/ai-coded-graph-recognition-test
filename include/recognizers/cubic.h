#ifndef GRAPH_RECOGNITION_CUBIC_H
#define GRAPH_RECOGNITION_CUBIC_H

/**
 * @file cubic.h
 * @brief Cubic graph recognition
 *
 * Verifies that all vertices have degree 3.
 */

#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for cubic graph recognition
 */
enum class CubicAlgorithm {
    DEGREE_CHECK /**< degree check */
};

/**
 * @brief Result of cubic graph recognition
 */
struct CubicResult {
    bool is_cubic = false; /**< true if the graph is cubic */
};

/**
 * @brief Determines whether a graph is a cubic graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DEGREE_CHECK)
 * @return CubicResult
 *
 * Cubic graph iff all vertices have degree 3.
 */
inline CubicResult check_cubic(const Graph& g,
    CubicAlgorithm algo = CubicAlgorithm::DEGREE_CHECK) {
    (void)algo;
    CubicResult res;

    int n = g.n;
    if (n == 0) return res;

    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    res.is_cubic = true;
    return res;
}

} // namespace graph_recognition

#endif
