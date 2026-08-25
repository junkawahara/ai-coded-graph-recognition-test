#ifndef GRAPH_RECOGNITION_PERMUTATION_H
#define GRAPH_RECOGNITION_PERMUTATION_H

/**
 * @file permutation.h
 * @brief Permutation graph recognition
 *
 * Determines whether the graph is a permutation graph by checking that
 * both G and its complement are comparability graphs.
 *
 * Algorithms:
 *   - BACKTRACKING: Transitive orientation via backtracking
 *   - CLASS_BASED: Orientation by Gamma class (O(nm), default)
 */

#include "graph.h"
#include "graph_utils.h"
#include "transitive_orientation.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for permutation graph recognition
 */
enum class PermutationAlgorithm {
    BACKTRACKING, /**< Transitive orientation via backtracking */
    CLASS_BASED   /**< Orientation by Gamma class (default) */
};

/**
 * @brief Result of permutation graph recognition
 */
struct PermutationResult {
    bool is_permutation = false; /**< true if the graph is a permutation graph */
};

namespace detail {

/**
 * @brief Permutation graph recognition via backtracking
 */
inline PermutationResult check_permutation_backtracking(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph(c)) return res;

    res.is_permutation = true;
    return res;
}

/**
 * @brief Permutation graph recognition via class-based method
 */
inline PermutationResult check_permutation_class_based(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph_class_based(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph_class_based(c)) return res;

    res.is_permutation = true;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a permutation graph
 * @param g Input graph
 * @param algo Algorithm to use (default: CLASS_BASED)
 * @return PermutationResult
 */
inline PermutationResult check_permutation(const Graph& g,
    PermutationAlgorithm algo = PermutationAlgorithm::CLASS_BASED) {
    switch (algo) {
        case PermutationAlgorithm::BACKTRACKING:
            return detail::check_permutation_backtracking(g);
        case PermutationAlgorithm::CLASS_BASED:
            return detail::check_permutation_class_based(g);
        default:
            break;
    }
    return PermutationResult();
}

} // namespace graph_recognition

#endif
