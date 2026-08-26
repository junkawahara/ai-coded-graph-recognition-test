#ifndef GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_H
#define GRAPH_RECOGNITION_SIMPLE_QUADRANGULATION_H

/**
 * @file simple_quadrangulation.h
 * @brief Simple quadrangulation recognition
 *
 * Determined by 3-connected + planar + m = 2n - 4 + triangle-free.
 * For 3-connected planar graphs, the embedding is unique by Whitney's theorem.
 * If m = 2n - 4 then the average face size is 4, and if triangle-free then all faces are quadrilaterals.
 */

#include "util/graph.h"
#include "recognizers/planar.h"
#include "recognizers/triconnected.h"
#include "recognizers/triangle_free.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for simple quadrangulation recognition
 */
enum class SimpleQuadrangulationAlgorithm {
    COMBINED /**< 3-connected + planar + edge count + triangle-free */
};

/**
 * @brief Result of simple quadrangulation recognition
 */
struct SimpleQuadrangulationResult {
    bool is_simple_quadrangulation = false; /**< true if the graph is a simple quadrangulation */
};

/**
 * @brief Determines whether the graph is a simple quadrangulation
 * @param g Input graph
 * @param algo Algorithm to use (default: COMBINED)
 * @return SimpleQuadrangulationResult
 *
 * Simple quadrangulation <=> 3-connected planar graph with all faces being quadrilaterals.
 * Determination: 3-connected AND planar AND m = 2n - 4 AND triangle-free.
 */
inline SimpleQuadrangulationResult check_simple_quadrangulation(const Graph& g,
    SimpleQuadrangulationAlgorithm algo = SimpleQuadrangulationAlgorithm::COMBINED) {
    (void)algo;
    SimpleQuadrangulationResult res;

    int n = g.n;
    if (n < 8) return res; /* minimum 3-connected quadrangulation is the cube, n=8 */

    /* Edge count check: m = 2n - 4 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 2LL * n - 4) return res;

    /* Triangle-free check (fast filter) */
    TriangleFreeResult tfr = check_triangle_free(g);
    if (!tfr.is_triangle_free) return res;

    /* Triconnected check */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* Planarity check */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_simple_quadrangulation = true;
    return res;
}

} // namespace graph_recognition

#endif
