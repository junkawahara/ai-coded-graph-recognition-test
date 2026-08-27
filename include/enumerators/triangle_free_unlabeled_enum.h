#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_UNLABELED_ENUM_H

/**
 * @file triangle_free_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic triangle-free graphs
 *
 * Enumerates one representative per isomorphism class of triangle-free
 * graphs on n vertices by McKay's canonical construction path method, the
 * algorithm behind geng -t. Graphs are grown one vertex at a time; adding a
 * vertex keeps the graph triangle-free if and only if its neighborhood is an
 * independent set, so the triangle-forbidding pruning is an independence
 * test on the candidate neighborhood, with no recognizer call.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic triangle-free graphs on n vertices =
 * OEIS A006785(n): 1, 2, 3, 7, 14, 38, 107, 410, 1897, 12172, ...
 * The connected ones are counted by A024607: 1, 1, 1, 3, 6, 19, 59, 267, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path; geng -t);
 *   Colbourn, Read, "Orderly algorithms for generating restricted classes
 *   of graphs," J. Graph Theory 3(2), 1979; OEIS A006785, A024607
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic triangle-free enumeration
 */
enum class TriangleFreeUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (geng -t style) */
};

/**
 * @brief An enumerated triangle-free graph
 */
struct TriangleFreeUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic triangle-free enumeration
 */
struct TriangleFreeUnlabeledEnumerationResult {
    std::vector<TriangleFreeUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline TriangleFreeUnlabeledEnumeratedGraph triangle_free_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    TriangleFreeUnlabeledEnumeratedGraph g;
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
 * Extends by a new vertex whose neighborhood S runs over the independent
 * subsets of the current vertex set (independence of S is exactly what keeps
 * the graph triangle-free). A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void triangle_free_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<TriangleFreeUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(triangle_free_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        bool independent = true;
        for (int u = 0; u < k && independent; ++u) {
            if (((s >> u) & 1ULL) && (adj[u] & s) != 0) independent = false;
        }
        if (!independent) continue;

        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(k + 1, adj);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            triangle_free_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
                                             results);
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic triangle-free graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return TriangleFreeUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A006785(n) graphs
 * (1, 2, 3, 7, 14, 38, 107, 410, ... for n = 1, 2, ...), or A024607(n)
 * (1, 1, 1, 3, 6, 19, 59, 267, ...) with @p connected_only. n = 0 yields
 * the single empty graph in both modes; negative n and n >= 64 yield
 * nothing.
 *
 * @note The canonicalization is exact branch-and-bound over vertex
 *       orderings (worst case k! on vertex-transitive graphs such as the
 *       empty graph), so the enumeration is practical to about n = 10
 *       (n = 9 takes about 2 seconds, n = 10 about half a minute;
 *       A006785(10) = 12172).
 */
inline TriangleFreeUnlabeledEnumerationResult
enumerate_triangle_free_unlabeled_graphs(
    int n, bool connected_only = false,
    TriangleFreeUnlabeledEnumAlgorithm algo =
        TriangleFreeUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    TriangleFreeUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        TriangleFreeUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::triangle_free_unlabeled_enum_dfs(1, adj, n, connected_only,
                                             result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
