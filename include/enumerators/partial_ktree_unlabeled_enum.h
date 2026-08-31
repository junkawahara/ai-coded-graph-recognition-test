#ifndef GRAPH_RECOGNITION_PARTIAL_KTREE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_PARTIAL_KTREE_UNLABELED_ENUM_H

/**
 * @file partial_ktree_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic partial k-trees (treewidth <= k)
 *
 * Enumerates one representative per isomorphism class of the graphs of
 * treewidth at most k on n vertices by McKay's canonical construction
 * path method — the survey's "geng + treewidth filter" route realized on
 * the shared canonical-augmentation machinery. Graphs are grown one
 * vertex at a time, which is sound because partial k-trees are
 * minor-closed and in particular hereditary: deleting the canonically
 * last vertex of a partial k-tree again yields a partial k-tree, so
 * every class is reachable from the one-vertex graph and the recognizer
 * (`check_partial_ktree`, the memoized elimination-order search) can
 * prune every candidate child before the far more expensive
 * canonicalization runs.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization
 * of each candidate child yields both its canonical form and the
 * automorphism orbit of the vertex placed last by the canonical
 * labeling, and the child survives only when the newly added vertex lies
 * in that orbit (so each class accepts exactly one parent class), with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * k = 1 reproduces the forests (OEIS A005195: 1, 2, 3, 6, 10, 20, 37,
 * ...; connected = trees, A000055) and k = 2 the series-parallel
 * (K4-minor-free) graphs, matching `series_parallel_unlabeled_enum.h`
 * (1, 2, 4, 10, 27, 92, 360, ...). k >= n-1 yields all graphs
 * (A000088: 1, 2, 4, 11, 34, 156, ...). The treewidth <= 3 counts
 * (1, 2, 4, 11, 33, 145, 861, 7604, ... for n = 1, 2, ...) are not in
 * the OEIS (verified against a brute-force treewidth filter through
 * n = 6).
 *
 * References:
 *   Dinneen, "Practical enumeration methods for graphs of bounded
 *   pathwidth and treewidth," CDMTCS-055, 1997 (the survey's dedicated
 *   constructive enumeration; the bounds there do not apply to this
 *   generic route);
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Arnborg, Corneil, Proskurowski, SIAM J. Alg. Disc. Meth. 8, 1987
 *   (the recognizer);
 *   OEIS A005195, A000055, A000088
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/partial_ktree.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic partial k-tree enumeration
 */
enum class PartialKTreeUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated partial k-tree
 */
struct PartialKTreeUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic partial k-tree enumeration
 */
struct PartialKTreeUnlabeledEnumerationResult {
    std::vector<PartialKTreeUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline PartialKTreeUnlabeledEnumeratedGraph partial_ktree_unlabeled_build_graph(
    int c, const std::vector<unsigned long long>& adj) {
    PartialKTreeUnlabeledEnumeratedGraph g;
    g.n = c;
    g.edges = bitmask_graph_edges(c, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a c-vertex canonical representative
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param k Treewidth bound
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over all subsets of
 * the current vertex set, keeping those for which the extended graph
 * still has treewidth <= k. A child survives when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class
 * is McKay's canonical construction path argument. Connectivity cannot
 * be pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void partial_ktree_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k,
    bool connected_only,
    std::vector<PartialKTreeUnlabeledEnumeratedGraph>& results) {
    if (c == n) {
        if (!connected_only || bitmask_graph_connected(c, adj)) {
            results.push_back(partial_ktree_unlabeled_build_graph(c, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << c;
    for (unsigned long long s = 0; s < limit; ++s) {
        adj.push_back(s);
        for (int u = 0; u < c; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << c;
        }

        Graph child(c + 1, bitmask_graph_edges(c + 1, adj));
        if (check_partial_ktree(child, k).is_partial_ktree) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(c + 1, adj);
            if (((canon.last_orbit >> c) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                partial_ktree_unlabeled_enum_dfs(c + 1, adj, n, k,
                                                 connected_only, results);
            }
        }

        for (int u = 0; u < c; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << c);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic partial k-trees on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param k Treewidth bound (the class parameter)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return PartialKTreeUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of the graphs of
 * treewidth <= k. k = 0 yields only the edgeless graph (none in
 * connected mode for n >= 2); k >= n-1 yields all graphs (A000088).
 * n = 0 yields the single empty graph in both modes (it has treewidth
 * -1, hence lies in the class for every k >= 0); negative n, n >= 64
 * and negative k yield nothing.
 *
 * @note Every candidate child costs one treewidth <= k recognition
 *       (exponential worst case, but cheap at these sizes) plus, when it
 *       passes, one exact canonicalization (branch and bound over vertex
 *       orderings, worst case c! on vertex-transitive graphs), so the
 *       enumeration is practical to about n = 8.
 */
inline PartialKTreeUnlabeledEnumerationResult
enumerate_partial_ktree_unlabeled_graphs(
    int n, int k, bool connected_only = false,
    PartialKTreeUnlabeledEnumAlgorithm algo =
        PartialKTreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    PartialKTreeUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64 || k < 0) return result;
    if (n == 0) {
        PartialKTreeUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::partial_ktree_unlabeled_enum_dfs(1, adj, n, k, connected_only,
                                             result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
