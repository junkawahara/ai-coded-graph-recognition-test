#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_H

/**
 * @file maximal_planar.h
 * @brief Maximal planar graph recognition
 *
 * Recognition via planarity check and edge count verification (m = 3n - 6).
 */

#include "graph.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for maximal planar graph recognition
 */
enum class MaximalPlanarAlgorithm {
    PLANAR_EDGE_COUNT /**< Planarity + edge count check */
};

/**
 * @brief Result of maximal planar graph recognition
 */
struct MaximalPlanarResult {
    bool is_maximal_planar = false; /**< true if the graph is maximal planar */
};

/**
 * @brief Determines whether the graph is a maximal planar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: PLANAR_EDGE_COUNT)
 * @return MaximalPlanarResult
 *
 * Maximal planar graph iff planar and m = 3n - 6 (n >= 3).
 * For n <= 2: K1, K2 are maximal planar.
 */
inline MaximalPlanarResult check_maximal_planar(const Graph& g,
    MaximalPlanarAlgorithm algo = MaximalPlanarAlgorithm::PLANAR_EDGE_COUNT) {
    (void)algo;
    MaximalPlanarResult res;

    int n = g.n;

    /* n <= 2: K0 (empty), K1, K2 are maximal planar */
    if (n <= 2) {
        long long m = 0;
        for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
        m /= 2;
        /* n=0: m=0 -> maximal planar */
        /* n=1: m=0 -> maximal planar */
        /* n=2: m=1 -> maximal planar, m=0 -> not maximal (can add edges) */
        if (n == 0 || n == 1) {
            res.is_maximal_planar = true;
        } else if (n == 2 && m == 1) {
            res.is_maximal_planar = true;
        }
        return res;
    }

    /* Edge count check: m = 3n - 6 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 3LL * n - 6) return res;

    /* Planarity check */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_maximal_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
