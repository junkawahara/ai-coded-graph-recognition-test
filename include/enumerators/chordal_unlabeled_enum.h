#ifndef GRAPH_RECOGNITION_CHORDAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CHORDAL_UNLABELED_ENUM_H

/**
 * @file chordal_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic chordal graphs
 *
 * Enumerates one representative per isomorphism class of chordal graphs on
 * n vertices by McKay's canonical construction path method. Graphs are
 * grown one vertex at a time, which is sound because chordal graphs are
 * hereditary: deleting the canonically last vertex of a chordal graph
 * again yields a chordal graph, so every class is reachable from the
 * one-vertex graph.
 *
 * The pruning is a recognizer call (`check_chordal`, linear-time
 * MCS + PEO verification) on every candidate child, applied before the
 * isomorph rejection so the far more expensive canonicalization only ever
 * runs on chordal graphs. (Adding a vertex whose neighborhood is a clique
 * always preserves chordality, but the converse fails — the new vertex
 * need not be simplicial — so the recognizer call cannot be replaced by a
 * clique test on the neighborhood.)
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic chordal graphs on n vertices =
 * OEIS A048193(n): 1, 2, 4, 10, 27, 94, 393, 2119, 14524, ...
 * The connected ones are
 * OEIS A048192(n): 1, 1, 2, 5, 15, 58, 272, 1614, 11911, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Tarjan, Yannakakis, "Simple linear-time algorithms to test chordality
 *   of graphs...," SIAM J. Comput. 13, 1984 (the recognizer);
 *   OEIS A048193, A048192
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/chordal.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic chordal enumeration
 */
enum class ChordalUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated chordal graph
 */
struct ChordalUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic chordal enumeration
 */
struct ChordalUnlabeledEnumerationResult {
    std::vector<ChordalUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline ChordalUnlabeledEnumeratedGraph chordal_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    ChordalUnlabeledEnumeratedGraph g;
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
 * chordal. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void chordal_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<ChordalUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(chordal_unlabeled_build_graph(k, adj));
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
        if (check_chordal(child).is_chordal) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                chordal_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic chordal graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return ChordalUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A048193(n) graphs
 * (1, 2, 4, 10, 27, 94, 393, 2119, ... for n = 1, 2, ...), or the
 * connected ones among them (A048192: 1, 1, 2, 5, 15, 58, 272, 1614, ...)
 * with @p connected_only. n = 0 yields the single empty graph in both
 * modes; negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one linear-time chordality recognition
 *       plus, when it is chordal, one exact canonicalization (branch and
 *       bound over vertex orderings, worst case k! on vertex-transitive
 *       graphs), so the enumeration is practical to about n = 9.
 */
inline ChordalUnlabeledEnumerationResult
enumerate_chordal_unlabeled_graphs(
    int n, bool connected_only = false,
    ChordalUnlabeledEnumAlgorithm algo =
        ChordalUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    ChordalUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        ChordalUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::chordal_unlabeled_enum_dfs(1, adj, n, connected_only,
                                       result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
