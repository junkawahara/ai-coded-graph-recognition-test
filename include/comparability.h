#ifndef GRAPH_RECOGNITION_COMPARABILITY_H
#define GRAPH_RECOGNITION_COMPARABILITY_H

/**
 * @file comparability.h
 * @brief Comparability graph recognition
 *
 * Recognizes comparability graphs using a transitive orientation solver.
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"
#include "transitive_orientation.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for comparability graph recognition
 */
enum class ComparabilityAlgorithm {
    TRANSITIVE_ORIENTATION /**< transitive orientation (Gamma-class based) */
};

/**
 * @brief Result of comparability graph recognition
 */
struct ComparabilityResult {
    bool is_comparability = false; /**< true if the graph is a comparability graph */
    /**
     * @brief A transitive orientation: each edge once, oriented u -> v
     *
     * Valid only when is_comparability == true. It is the partial order whose
     * comparability graph is g.
     */
    std::vector<std::pair<int, int>> orientation;
    /**
     * @brief NO certificate: a FORCING_CYCLE
     *
     * Valid only when is_comparability == false. Left empty by the recognizer:
     * the solver decides by propagating a mixture of Gamma steps, transitivity
     * and earlier fixed classes, and only the pure Gamma part is
     * independently checkable. build_comparability_obstruction() derives that
     * part from scratch.
     */
    Obstruction obstruction;
};

/**
 * @brief Determines whether a graph is a comparability graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only TRANSITIVE_ORIENTATION is implemented)
 * @return ComparabilityResult
 *
 * A graph is a comparability graph if it admits a transitive orientation of its edges.
 * The orientation found is returned alongside the answer; see
 * transitive_orientation.h for the matrix form and the algorithm choice.
 */
inline ComparabilityResult check_comparability(const Graph& g,
    ComparabilityAlgorithm algo = ComparabilityAlgorithm::TRANSITIVE_ORIENTATION) {
    (void)algo;
    ComparabilityResult res;

    TransitiveOrientationResult to = transitive_orientation(g);
    if (!to.is_comparability) return res;

    res.orientation.swap(to.orientation);
    res.is_comparability = true;
    return res;
}

/**
 * @brief Builds a NO certificate for a non-comparability graph
 * @param g Input graph
 * @return A FORCING_CYCLE, or an empty obstruction if g is a comparability graph
 *
 * Rebuilds the Gamma-forcing relation directly rather than reading the
 * recognizer's propagation trail, so the chain it returns can be replayed step
 * by step from the definition alone.
 */
inline Obstruction build_comparability_obstruction(const Graph& g) {
    return detail_obstruction::find_forcing_cycle(g);
}

} // namespace graph_recognition

#endif
