#ifndef GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_PERMUTATION_UNLABELED_ENUM_H

/**
 * @file bipartite_permutation_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic bipartite permutation graphs
 *
 * Enumerates one representative per isomorphism class of bipartite
 * permutation graphs (graphs that are both bipartite and permutation
 * graphs) on n vertices by McKay's canonical construction path method.
 * Graphs are grown one vertex at a time, which is sound because the
 * class is hereditary (both bipartiteness and being a permutation graph
 * are preserved by vertex deletion), so every class is reachable from
 * the one-vertex graph.
 *
 * The pruning is a recognizer call (`check_bipartite_permutation`, a
 * bipartiteness test plus the generic permutation graph test) on every
 * candidate child, applied before the isomorph rejection so the far more
 * expensive canonicalization only ever runs on bipartite permutation
 * graphs. (The dedicated enumerator of Saitoh, Otachi, Yamanaka and
 * Uehara enumerates the connected members through a canonical string
 * encoding without isomorph rejection, and the BDD-based construction
 * of Kawahara, Saitoh, Takeda, Yoshinaka and Yoshioka enumerates the
 * non-isomorphic members in time polynomial in n; this implementation
 * instead reuses the shared
 * canonical-augmentation machinery, which is exact and practical to
 * about n = 10, so those bounds do not apply here.)
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Saitoh, Otachi, Yamanaka, Uehara, "Random generation and enumeration
 *   of bipartite permutation graphs," J. Discrete Algorithms 10, 2012
 *   (ISAAC 2009; the dedicated O(1)-delay connected enumerator and O(n)
 *   uniform random generator);
 *   Kawahara, Saitoh, Takeda, Yoshinaka, Yoshioka, TCS 1003, 2024
 *   (BDD-based polynomial-time non-isomorphic enumeration)
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/bipartite_permutation.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic bipartite permutation enumeration
 */
enum class BipartitePermutationUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated bipartite permutation graph
 */
struct BipartitePermutationUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic bipartite permutation enumeration
 */
struct BipartitePermutationUnlabeledEnumerationResult {
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline BipartitePermutationUnlabeledEnumeratedGraph
bipartite_permutation_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    BipartitePermutationUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over all subsets of the
 * current vertex set, keeping those for which the extended graph is still
 * a bipartite permutation graph. A child survives when the new vertex lies
 * in its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument. Connectivity cannot be
 * pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void bipartite_permutation_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<BipartitePermutationUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(
                bipartite_permutation_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        Graph child(k + 1, bitmask_graph_edges(k + 1, adj));
        if (check_bipartite_permutation(child).is_bipartite_permutation) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                bipartite_permutation_unlabeled_enum_dfs(
                    k + 1, adj, n, connected_only, results);
            }
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic bipartite permutation graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return BipartitePermutationUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of bipartite permutation
 * graphs, or the connected ones among them with @p connected_only.
 * n = 0 yields the single empty graph in both modes; negative n and
 * n >= 64 yield nothing.
 *
 * @note Every candidate child costs one bipartite permutation recognition
 *       (a bipartiteness test plus the generic permutation graph test)
 *       plus, when it passes, one exact canonicalization (branch and
 *       bound over vertex orderings), so the enumeration is practical to
 *       about n = 10.
 */
inline BipartitePermutationUnlabeledEnumerationResult
enumerate_bipartite_permutation_unlabeled_graphs(
    int n, bool connected_only = false,
    BipartitePermutationUnlabeledEnumAlgorithm algo =
        BipartitePermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    BipartitePermutationUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        BipartitePermutationUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::bipartite_permutation_unlabeled_enum_dfs(1, adj, n, connected_only,
                                                     result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
