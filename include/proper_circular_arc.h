#ifndef GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H
#define GRAPH_RECOGNITION_PROPER_CIRCULAR_ARC_H

/**
 * @file proper_circular_arc.h
 * @brief Proper circular-arc graph recognition
 *
 * A proper circular-arc graph is a graph with a circular-arc representation
 * where no arc properly contains another.
 *
 * NOTE: "circular-arc AND claw-free" is NOT a valid characterization
 * (Roberts' theorem for proper interval graphs does not carry over to the
 * circular case): the net graph (a triangle with three pendant vertices) is
 * circular-arc and claw-free but has no proper circular-arc model.
 *
 * Algorithm: endpoint-order backtracking (shared with circular_arc.h) with
 * the additional 2-SAT constraint that no chosen arc orientation lets one
 * arc contain another (exponential time, for small graphs). Claw-freeness
 * is used only as a fast necessary filter: K_{1,3} itself has no proper
 * circular-arc model and the class is hereditary.
 *
 * References:
 *   - Tucker (1974), "Structure theorems for some circular-arc graphs"
 *   - Deng, Hell, Huang (1996), O(n+m) recognition (not implemented here)
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
 */
inline ProperCircularArcResult check_proper_circular_arc(const Graph& g) {
    ProperCircularArcResult res;

    /* Fast necessary filter: proper circular-arc graphs are claw-free
       (K_{1,3} has no proper circular-arc model; hereditary class). */
    ClawFreeResult cf = check_claw_free(g);
    if (!cf.is_claw_free) return res;

    /* Search for an arc model in which no arc contains another. */
    CircularArcResult ca =
        detail_circular_arc::check_circular_arc_backtracking(g, /*proper=*/true);
    res.is_proper_circular_arc = ca.is_circular_arc;
    return res;
}

}  // namespace graph_recognition

#endif
