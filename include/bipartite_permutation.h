#ifndef GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_H
#define GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_H

/**
 * @file bipartite_permutation.h
 * @brief Bipartite permutation graph recognition
 *
 * A bipartite permutation graph is a graph that is both bipartite and a permutation graph.
 */

#include "bipartite.h"
#include "graph.h"
#include "permutation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for bipartite permutation graph recognition
 */
enum class BipartitePermutationAlgorithm {
    CHAIN_BOTH_SIDES /**< bipartiteness + permutation graph test */
};

/**
 * @brief Result of bipartite permutation graph recognition
 */
struct BipartitePermutationResult {
    bool is_bipartite_permutation = false; /**< true if the graph is a bipartite permutation graph */
};

/**
 * @brief Determines whether a graph is a bipartite permutation graph
 * @param g Input graph
 * @param algo Algorithm selector (currently only CHAIN_BOTH_SIDES is implemented)
 * @return BipartitePermutationResult
 *
 * G is a bipartite permutation graph iff G is bipartite and a permutation graph.
 */
inline BipartitePermutationResult check_bipartite_permutation(const Graph& g,
    BipartitePermutationAlgorithm algo = BipartitePermutationAlgorithm::CHAIN_BOTH_SIDES) {
    (void)algo;
    BipartitePermutationResult res;
    res.is_bipartite_permutation = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    PermutationResult perm = check_permutation(g);
    if (!perm.is_permutation) return res;

    res.is_bipartite_permutation = true;
    return res;
}

} // namespace graph_recognition

#endif
