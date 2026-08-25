#ifndef GRAPH_RECOGNITION_COMPARABILITY_H
#define GRAPH_RECOGNITION_COMPARABILITY_H

/**
 * @file comparability.h
 * @brief Comparability graph recognition
 *
 * Recognizes comparability graphs using a transitive orientation solver.
 */

#include "graph.h"
#include "permutation.h"
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
};

/**
 * @brief Determines whether a graph is a comparability graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only TRANSITIVE_ORIENTATION is implemented)
 * @return ComparabilityResult
 *
 * A graph is a comparability graph if it admits a transitive orientation of its edges.
 */
inline ComparabilityResult check_comparability(const Graph& g,
    ComparabilityAlgorithm algo = ComparabilityAlgorithm::TRANSITIVE_ORIENTATION) {
    (void)algo;
    ComparabilityResult res;
    res.is_comparability = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!detail::is_comparability_graph_class_based(a)) return res;

    res.is_comparability = true;
    return res;
}

} // namespace graph_recognition

#endif
