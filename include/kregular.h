#ifndef GRAPH_RECOGNITION_KREGULAR_H
#define GRAPH_RECOGNITION_KREGULAR_H

/**
 * @file kregular.h
 * @brief k-regular graph recognition
 *
 * Checks that all vertices have the same degree.
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for k-regular graph recognition
 */
enum class KRegularAlgorithm {
    DEGREE_CHECK /**< Degree check */
};

/**
 * @brief Result of k-regular graph recognition
 */
struct KRegularResult {
    bool is_kregular = false; /**< true if the graph is k-regular */
    int k = -1;              /**< Regular degree (-1 if not regular) */
};

/**
 * @brief Determines whether the graph is a regular graph
 * @param g Input graph
 * @param algo Algorithm to use (default: DEGREE_CHECK)
 * @return KRegularResult
 *
 * Regular graph iff all vertices have the same degree.
 */
inline KRegularResult check_kregular(const Graph& g,
    KRegularAlgorithm algo = KRegularAlgorithm::DEGREE_CHECK) {
    (void)algo;
    KRegularResult res;

    int n = g.n;
    if (n == 0) {
        res.is_kregular = true;
        res.k = 0;
        return res;
    }

    int deg = (int)g.adj[1].size();
    for (int v = 2; v <= n; ++v) {
        if ((int)g.adj[v].size() != deg) return res;
    }

    res.is_kregular = true;
    res.k = deg;
    return res;
}

} // namespace graph_recognition

#endif
