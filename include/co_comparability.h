#ifndef GRAPH_RECOGNITION_CO_COMPARABILITY_H
#define GRAPH_RECOGNITION_CO_COMPARABILITY_H

/**
 * @file co_comparability.h
 * @brief Co-comparability graph recognition
 *
 * Determines the graph is co-comparability if the complement graph is a comparability graph.
 */

#include "comparability.h"
#include "graph.h"
#include "permutation.h"
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
};

/**
 * @brief Determines whether a graph is a co-comparability graph
 * @param g Input graph
 * @return CoComparabilityResult
 *
 * G is co-comparability iff complement(G) is a comparability graph.
 */
inline CoComparabilityResult check_co_comparability(const Graph& g,
    CoComparabilityAlgorithm algo = CoComparabilityAlgorithm::COMPLEMENT) {
    (void)algo;
    CoComparabilityResult res;
    res.is_co_comparability = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);
    if (!detail::is_comparability_graph_class_based(c)) return res;

    res.is_co_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
