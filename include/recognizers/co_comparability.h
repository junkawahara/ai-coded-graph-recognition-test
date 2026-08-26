#ifndef GRAPH_RECOGNITION_CO_COMPARABILITY_H
#define GRAPH_RECOGNITION_CO_COMPARABILITY_H

/**
 * @file co_comparability.h
 * @brief Co-comparability graph recognition
 *
 * Determines the graph is co-comparability if the complement graph is a comparability graph.
 */

#include "recognizers/comparability.h"
#include "util/graph.h"
#include "util/graph_utils.h"
#include "decompositions/transitive_orientation.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-comparability graph recognition
 */
enum class CoComparabilityAlgorithm {
    COMPLEMENT /**< comparability test on the complement graph */
};

/**
 * @brief Result of co-comparability graph recognition
 */
struct CoComparabilityResult {
    bool is_co_comparability = false; /**< true if the graph is co-comparability */
    Obstruction obstruction; /**< NO certificate: a FORCING_CYCLE of the complement,
                                  with in_complement set. Left empty by the
                                  recognizer for the same reason as comparability;
                                  build_co_comparability_obstruction() produces it.
                                  Valid only when is_co_comparability == false */
};

/**
 * @brief Determines whether a graph is a co-comparability graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only COMPLEMENT is implemented)
 * @return CoComparabilityResult
 *
 * G is co-comparability iff complement(G) is a comparability graph.
 */
inline CoComparabilityResult check_co_comparability(const Graph& g,
    CoComparabilityAlgorithm algo = CoComparabilityAlgorithm::COMPLEMENT) {
    (void)algo;
    CoComparabilityResult res;
    res.is_co_comparability = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!transitive_orientation_matrix(c).is_comparability) return res;

    res.is_co_comparability = true;
    return res;
}

/**
 * @brief Builds a NO certificate for a non-co-comparability graph
 * @param g Input graph
 * @return A FORCING_CYCLE of the complement with in_complement set, or an empty
 *         obstruction if g is a co-comparability graph
 */
inline Obstruction build_co_comparability_obstruction(const Graph& g) {
    Obstruction o = detail_obstruction::find_forcing_cycle(build_complement(g));
    if (o.has_witness()) o.in_complement = true;
    return o;
}

} // namespace graph_recognition

#endif
