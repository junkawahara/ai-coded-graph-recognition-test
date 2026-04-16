#ifndef GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H
#define GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H

/**
 * @file proper_circular_arc.h
 * @brief Proper circular-arc graph recognition
 *
 * A proper circular-arc graph is a circular-arc graph that has a representation
 * where no arc properly contains another.
 *
 * Characterization: G is a proper circular-arc graph <=> G is a circular-arc graph and claw-free (K_{1,3}-free)
 *
 * This is analogous to proper interval graphs (interval and claw-free).
 *
 * References:
 *   - Tucker (1974), "Structure theorems for some circular-arc graphs"
 *   - Deng, Hell, Huang (1996), O(n+m) recognition
 *   - Lin, Soulignac, Szwarcfiter (2013), certifying linear time
 */

#include "graph.h"
#include "circular_arc.h"
#include "claw_free.h"

namespace graph_recognition {

/**
 * @brief Result of proper circular-arc graph recognition
 */
struct ProperCircularArcResult {
    bool is_proper_circular_arc = false; /**< true if the graph is a proper circular-arc graph */
};

/**
 * @brief Determines whether the graph is a proper circular-arc graph
 * @param g Input graph
 * @return ProperCircularArcResult
 *
 * G is a proper circular-arc graph <=> G is a circular-arc graph and claw-free.
 */
inline ProperCircularArcResult check_proper_circular_arc(const Graph& g) {
    ProperCircularArcResult res;

    CircularArcResult ca = check_circular_arc(g);
    if (!ca.is_circular_arc) return res;

    ClawFreeResult cf = check_claw_free(g);
    if (!cf.is_claw_free) return res;

    res.is_proper_circular_arc = true;
    return res;
}

}  // namespace graph_recognition

#endif
