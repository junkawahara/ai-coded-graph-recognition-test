#ifndef GRAPH_RECOGNITION_COMPARABILITY_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_COMPARABILITY_UNLABELED_ENUM_H

/**
 * @file comparability_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic comparability graphs
 *
 * Enumerates one representative per isomorphism class of comparability
 * graphs on n vertices by McKay's canonical construction path method.
 * Graphs are grown one vertex at a time, which is sound because
 * comparability graphs are hereditary: restricting a transitive
 * orientation to an induced subgraph keeps it transitive, so deleting the
 * canonically last vertex of a comparability graph again yields a
 * comparability graph and every class is reachable from the one-vertex
 * graph.
 *
 * The pruning is a recognizer call (`check_comparability`, Golumbic's
 * O(nm) Gamma-class transitive orientation) on every candidate child,
 * applied before the isomorph rejection so the far more expensive
 * canonicalization only ever runs on comparability graphs.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic comparability graphs on n vertices =
 * OEIS A123416(n): 1, 2, 4, 11, 33, 144, 824, 6793, 75400, ...
 * The connected counts (1, 1, 2, 6, 20, 101, 646, 5797, ...) are not in
 * the OEIS. By Möhring's theorem the count is asymptotically about half
 * the number of posets (A000112), since almost all comparability graphs
 * have a unique partial order (up to duality). Complementation makes
 * these also the co-comparability counts
 * (`co_comparability_unlabeled_enum.h`).
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Golumbic, "Algorithmic Graph Theory and Perfect Graphs," 1980
 *   (the transitive orientation recognizer);
 *   Möhring, "Almost all comparability graphs are UPO," Discrete Math. 50,
 *   1984;
 *   OEIS A123416
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/comparability.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic comparability enumeration
 */
enum class ComparabilityUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated comparability graph
 */
struct ComparabilityUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic comparability enumeration
 */
struct ComparabilityUnlabeledEnumerationResult {
    std::vector<ComparabilityUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline ComparabilityUnlabeledEnumeratedGraph comparability_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    ComparabilityUnlabeledEnumeratedGraph g;
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
 * current vertex set, keeping those for which the extended graph still has
 * a transitive orientation. A child survives when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument. Connectivity cannot be
 * pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void comparability_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<ComparabilityUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(comparability_unlabeled_build_graph(k, adj));
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
        if (check_comparability(child).is_comparability) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                comparability_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
                                                 results);
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
 * @brief Enumerates all non-isomorphic comparability graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return ComparabilityUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A123416(n) graphs
 * (1, 2, 4, 11, 33, 144, 824, 6793, ... for n = 1, 2, ...), or only the
 * connected ones among them (1, 1, 2, 6, 20, 101, 646, 5797, ...) with
 * @p connected_only. n = 0 yields the single empty graph in both modes;
 * negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one O(nm) transitive orientation
 *       recognition plus, when it passes, one exact canonicalization
 *       (branch and bound over vertex orderings, worst case k! on
 *       vertex-transitive graphs), so the enumeration is practical to
 *       about n = 9 (n = 8 about a second, n = 9 about half a minute).
 */
inline ComparabilityUnlabeledEnumerationResult
enumerate_comparability_unlabeled_graphs(
    int n, bool connected_only = false,
    ComparabilityUnlabeledEnumAlgorithm algo =
        ComparabilityUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    ComparabilityUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        ComparabilityUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::comparability_unlabeled_enum_dfs(1, adj, n, connected_only,
                                             result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
