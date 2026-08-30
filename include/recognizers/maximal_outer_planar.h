#ifndef GRAPH_RECOGNITION_MAXIMAL_OUTER_PLANAR_H
#define GRAPH_RECOGNITION_MAXIMAL_OUTER_PLANAR_H

/**
 * @file maximal_outer_planar.h
 * @brief Maximal outerplanar graph recognition
 *
 * Recognition via outerplanarity check and edge count verification
 * (m = 2n - 3). A simple outerplanar graph on n >= 2 vertices has at most
 * 2n - 3 edges, so an outerplanar graph reaching that bound cannot accept
 * another edge; conversely every edge-maximal outerplanar graph on n >= 2
 * vertices is a triangulation of a convex polygon (for n >= 3: 2-connected,
 * a unique Hamiltonian outer cycle, every inner face a triangle) and has
 * exactly 2n - 3 edges. For n >= 2 the class coincides with the simple
 * 2-trees.
 */

#include "util/graph.h"
#include "recognizers/outer_planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for maximal outerplanar graph recognition
 */
enum class MaximalOuterPlanarAlgorithm {
    OUTER_PLANAR_EDGE_COUNT /**< Outerplanarity + edge count check */
};

/**
 * @brief Result of maximal outerplanar graph recognition
 */
struct MaximalOuterPlanarResult {
    bool is_maximal_outer_planar = false; /**< true if the graph is maximal outerplanar */
};

/**
 * @brief Determines whether the graph is a maximal outerplanar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: OUTER_PLANAR_EDGE_COUNT)
 * @return MaximalOuterPlanarResult
 *
 * Maximal outerplanar iff outerplanar and m = 2n - 3 (n >= 2).
 * For n <= 1: K0 and K1 are maximal outerplanar (no edge can be added).
 * n = 2 is covered by the edge-count formula (K2 has 1 = 2*2 - 3 edges;
 * the edgeless 2-vertex graph is not maximal).
 */
inline MaximalOuterPlanarResult check_maximal_outer_planar(const Graph& g,
    MaximalOuterPlanarAlgorithm algo =
        MaximalOuterPlanarAlgorithm::OUTER_PLANAR_EDGE_COUNT) {
    (void)algo;
    MaximalOuterPlanarResult res;

    int n = g.n;
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    if (n <= 1) {
        res.is_maximal_outer_planar = true;
        return res;
    }

    /* Edge count check: m = 2n - 3 */
    if (m != 2LL * n - 3) return res;

    /* Outerplanarity check */
    OuterPlanarResult opr = check_outer_planar(g);
    if (!opr.is_outer_planar) return res;

    res.is_maximal_outer_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
