#ifndef GRAPH_RECOGNITION_POLYHEDRAL_H
#define GRAPH_RECOGNITION_POLYHEDRAL_H

/**
 * @file polyhedral.h
 * @brief Polyhedral graph recognition
 *
 * Steinitz's theorem: polyhedral graph <=> 3-connected planar graph.
 */

#include "graph.h"
#include "planar.h"
#include "triconnected.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for polyhedral graph recognition
 */
enum class PolyhedralAlgorithm {
    STEINITZ /**< Steinitz's theorem (3-connected + planar) */
};

/**
 * @brief Result of polyhedral graph recognition
 */
struct PolyhedralResult {
    bool is_polyhedral = false; /**< true if the graph is a polyhedral graph */
};

/**
 * @brief Determines whether the graph is a polyhedral graph
 * @param g Input graph
 * @param algo Algorithm to use (default: STEINITZ)
 * @return PolyhedralResult
 *
 * Steinitz's theorem: polyhedral graph <=> 3-connected and planar.
 */
inline PolyhedralResult check_polyhedral(const Graph& g,
    PolyhedralAlgorithm algo = PolyhedralAlgorithm::STEINITZ) {
    (void)algo;
    PolyhedralResult res;

    /* Triconnected check (including minimum degree 3 pre-check) */
    TriconnectedResult tr = check_triconnected(g);
    if (!tr.is_triconnected) return res;

    /* Planarity check */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_polyhedral = true;
    return res;
}

} // namespace graph_recognition

#endif
