#ifndef GRAPH_RECOGNITION_CUBIC_PLANAR_H
#define GRAPH_RECOGNITION_CUBIC_PLANAR_H

/**
 * @file cubic_planar.h
 * @brief Cubic planar graph recognition
 *
 * Checks both 3-regularity and planarity.
 */

#include "util/graph.h"
#include "recognizers/cubic.h"
#include "recognizers/planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for cubic planar graph recognition
 */
enum class CubicPlanarAlgorithm {
    CUBIC_AND_PLANAR /**< cubic + planarity check */
};

/**
 * @brief Result of cubic planar graph recognition
 */
struct CubicPlanarResult {
    bool is_cubic_planar = false; /**< true if the graph is cubic planar */
};

/**
 * @brief Determines whether a graph is a cubic planar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: CUBIC_AND_PLANAR)
 * @return CubicPlanarResult
 *
 * Cubic planar graph iff every vertex has degree 3 and the graph is planar.
 */
inline CubicPlanarResult check_cubic_planar(const Graph& g,
    CubicPlanarAlgorithm algo = CubicPlanarAlgorithm::CUBIC_AND_PLANAR) {
    (void)algo;
    CubicPlanarResult res;

    /* Cubic (3-regular) check (can fail quickly) */
    CubicResult cr = check_cubic(g);
    if (!cr.is_cubic) return res;

    /* Planarity check */
    PlanarResult pr = check_planar(g);
    if (!pr.is_planar) return res;

    res.is_cubic_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
