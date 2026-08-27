#ifndef GRAPH_RECOGNITION_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_PLANAR_UNLABELED_ENUM_H

/**
 * @file planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic planar graphs
 *
 * Enumerates one representative per isomorphism class of planar graphs on
 * n vertices by McKay's canonical construction path method. Graphs are
 * grown one vertex at a time, which is sound because planar graphs are
 * hereditary: deleting the canonically last vertex of a planar graph
 * again yields a planar graph, so every class is reachable from the
 * one-vertex graph.
 *
 * The pruning is a recognizer call (`check_planar`, the linear-time
 * left-right planarity criterion) on every candidate child, applied before
 * the isomorph rejection so the far more expensive canonicalization only
 * ever runs on planar graphs.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic planar graphs on n vertices =
 * OEIS A005470(n): 1, 2, 4, 11, 33, 142, 822, 6966, 79853, ...
 * The connected ones are
 * OEIS A003094(n): 1, 1, 2, 6, 20, 99, 646, 5974, 71885, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Brandes, "The left-right planarity test," 2009 (the recognizer);
 *   OEIS A005470, A003094
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/planar.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic planar enumeration
 */
enum class PlanarUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated planar graph
 */
struct PlanarUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic planar enumeration
 */
struct PlanarUnlabeledEnumerationResult {
    std::vector<PlanarUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline PlanarUnlabeledEnumeratedGraph planar_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    PlanarUnlabeledEnumeratedGraph g;
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
 * planar. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void planar_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<PlanarUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(planar_unlabeled_build_graph(k, adj));
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
        if (check_planar(child).is_planar) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                planar_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic planar graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return PlanarUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A005470(n) graphs
 * (1, 2, 4, 11, 33, 142, 822, 6966, ... for n = 1, 2, ...), or the
 * connected ones among them (A003094: 1, 1, 2, 6, 20, 99, 646, 5974, ...)
 * with @p connected_only. n = 0 yields the single empty graph in both
 * modes; negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one linear-time planarity recognition
 *       plus, when it is planar, one exact canonicalization (branch and
 *       bound over vertex orderings, worst case k! on vertex-transitive
 *       graphs), so the enumeration is practical to about n = 8.
 */
inline PlanarUnlabeledEnumerationResult
enumerate_planar_unlabeled_graphs(
    int n, bool connected_only = false,
    PlanarUnlabeledEnumAlgorithm algo =
        PlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    PlanarUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        PlanarUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::planar_unlabeled_enum_dfs(1, adj, n, connected_only,
                                      result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
