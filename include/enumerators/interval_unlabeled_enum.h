#ifndef GRAPH_RECOGNITION_INTERVAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_INTERVAL_UNLABELED_ENUM_H

/**
 * @file interval_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic interval graphs
 *
 * Enumerates one representative per isomorphism class of interval graphs on
 * n vertices by McKay's canonical construction path method. Graphs are
 * grown one vertex at a time, which is sound because interval graphs are
 * hereditary: deleting the canonically last vertex of an interval graph
 * again yields an interval graph, so every class is reachable from the
 * one-vertex graph.
 *
 * The pruning is a recognizer call (`check_interval`, chordal + AT-free)
 * on every candidate child, applied before the isomorph rejection so the
 * far more expensive canonicalization only ever runs on interval graphs.
 * (The dedicated non-isomorphic interval enumerators of Yamazaki, Saitoh,
 * Kiyomi, and Uno — O(n^4) delay via MPQ-tree canonical forms, improved to
 * O(n^3 log n) delay by Mikos — avoid the canonicalization entirely; this
 * implementation instead reuses the shared canonical-augmentation
 * machinery, which is exact and practical to about n = 9.)
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic interval graphs on n vertices =
 * OEIS A005975(n): 1, 2, 4, 10, 27, 92, 369, 1807, 10344, ...
 * The connected ones are
 * OEIS A005976(n): 1, 1, 2, 5, 15, 56, 250, 1328, 8069, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Lekkerkerker, Boland, "Representation of a finite graph by a set of
 *   intervals on the real line," Fund. Math. 51, 1962 (the chordal +
 *   AT-free characterization behind the recognizer);
 *   Yamazaki, Saitoh, Kiyomi, Uno, "Enumeration of nonisomorphic interval
 *   graphs and nonisomorphic permutation graphs," WALCOM 2018 / Theor.
 *   Comput. Sci. 806, 2020 (the dedicated O(n^4)-delay algorithm);
 *   Mikos, "Efficient enumeration of non-isomorphic interval graphs,"
 *   Discrete Math. Theor. Comput. Sci. 23(1), 2021 (O(n^3 log n) delay);
 *   OEIS A005975, A005976
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/interval.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic interval enumeration
 */
enum class IntervalUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated interval graph
 */
struct IntervalUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic interval enumeration
 */
struct IntervalUnlabeledEnumerationResult {
    std::vector<IntervalUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline IntervalUnlabeledEnumeratedGraph interval_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    IntervalUnlabeledEnumeratedGraph g;
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
 * interval. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void interval_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<IntervalUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(interval_unlabeled_build_graph(k, adj));
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
        if (check_interval(child).is_interval) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                interval_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic interval graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return IntervalUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A005975(n) graphs
 * (1, 2, 4, 10, 27, 92, 369, 1807, ... for n = 1, 2, ...), or the
 * connected ones among them (A005976: 1, 1, 2, 5, 15, 56, 250, 1328, ...)
 * with @p connected_only. n = 0 yields the single empty graph in both
 * modes; negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one interval recognition (chordality,
 *       asteroidal-triple scan, and clique-path construction) plus, when it
 *       is interval, one exact
 *       canonicalization (branch and bound over vertex orderings, worst
 *       case k! on vertex-transitive graphs), so the enumeration is
 *       practical to about n = 9.
 */
inline IntervalUnlabeledEnumerationResult
enumerate_interval_unlabeled_graphs(
    int n, bool connected_only = false,
    IntervalUnlabeledEnumAlgorithm algo =
        IntervalUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    IntervalUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        IntervalUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::interval_unlabeled_enum_dfs(1, adj, n, connected_only,
                                        result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
