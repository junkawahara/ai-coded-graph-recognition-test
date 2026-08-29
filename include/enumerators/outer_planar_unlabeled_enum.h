#ifndef GRAPH_RECOGNITION_OUTER_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_OUTER_PLANAR_UNLABELED_ENUM_H

/**
 * @file outer_planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic outerplanar graphs
 *
 * Enumerates one representative per isomorphism class of outerplanar
 * graphs on n vertices by McKay's canonical construction path method.
 * Graphs are grown one vertex at a time, which is sound because
 * outerplanar graphs are hereditary: deleting the canonically last vertex
 * of an outerplanar graph again yields an outerplanar graph, so every
 * class is reachable from the one-vertex graph.
 *
 * The pruning is a recognizer call (`check_outer_planar`, the linear-time
 * left-right planarity test on G + K1) on every candidate child, applied
 * before the isomorph rejection so the far more expensive canonicalization
 * only ever runs on outerplanar graphs. (The dedicated enumerator of Wang
 * and Nagamochi generates rooted connected outerplanar graphs in O(1) time
 * per graph by growing the block tree directly and never canonicalizes;
 * this implementation instead reuses the shared canonical-augmentation
 * machinery, which is exact and practical to about n = 10, so that
 * survey's per-graph bound does not apply here.)
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic outerplanar graphs on n vertices =
 * OEIS A111564(n): 1, 2, 4, 10, 25, 80, 277, 1150, 5291, ...
 * The connected ones are
 * OEIS A111563(n): 1, 1, 2, 5, 13, 46, 172, 777, 3783, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Chartrand, Harary, "Planar permutation graphs," Ann. Inst. H.
 *   Poincare B 3, 1967 (outerplanar = K4-minor-free and K2,3-minor-free,
 *   behind the recognizer's G + K1 planarity criterion);
 *   Wang, Nagamochi, "Constant time generation of rooted and colored
 *   outerplanar graphs," AAIM 2010 (the dedicated O(1)-per-graph rooted
 *   enumerator);
 *   OEIS A111564, A111563
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/outer_planar.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic outerplanar enumeration
 */
enum class OuterPlanarUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated outerplanar graph
 */
struct OuterPlanarUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic outerplanar enumeration
 */
struct OuterPlanarUnlabeledEnumerationResult {
    std::vector<OuterPlanarUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline OuterPlanarUnlabeledEnumeratedGraph outer_planar_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    OuterPlanarUnlabeledEnumeratedGraph g;
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
 * outerplanar. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void outer_planar_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<OuterPlanarUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(outer_planar_unlabeled_build_graph(k, adj));
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
        if (check_outer_planar(child).is_outer_planar) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                outer_planar_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic outerplanar graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return OuterPlanarUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A111564(n) graphs
 * (1, 2, 4, 10, 25, 80, 277, 1150, ... for n = 1, 2, ...), or the
 * connected ones among them (A111563: 1, 1, 2, 5, 13, 46, 172, 777, ...)
 * with @p connected_only. n = 0 yields the single empty graph in both
 * modes; negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one outerplanarity recognition (a
 *       linear-time planarity test of the graph plus an apex vertex) plus,
 *       when it is outerplanar, one exact canonicalization (branch and
 *       bound over vertex orderings, worst case k! on vertex-transitive
 *       graphs such as the cycles), so the enumeration is practical to
 *       about n = 10.
 */
inline OuterPlanarUnlabeledEnumerationResult
enumerate_outer_planar_unlabeled_graphs(
    int n, bool connected_only = false,
    OuterPlanarUnlabeledEnumAlgorithm algo =
        OuterPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    OuterPlanarUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        OuterPlanarUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::outer_planar_unlabeled_enum_dfs(1, adj, n, connected_only,
                                            result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
