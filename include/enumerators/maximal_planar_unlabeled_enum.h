#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_UNLABELED_ENUM_H

/**
 * @file maximal_planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic maximal planar graphs (triangulations)
 *
 * Enumerates one representative per isomorphism class of maximal planar
 * graphs (planar with m = 3n - 6 for n >= 3; K1 and K2 below that) on n
 * vertices by McKay's canonical construction path method. The class
 * itself is not hereditary — deleting a vertex of a triangulation leaves
 * a planar graph that is no longer edge-maximal — but every vertex-deleted
 * subgraph is still planar, so the search grows *planar* graphs one vertex
 * at a time exactly as in `planar_unlabeled_enum.h` and emits the n-vertex
 * graphs whose edge count reaches 3n - 6.
 *
 * Two prunings run on every candidate child before the isomorph
 * rejection: a `check_planar` call (the linear-time left-right planarity
 * criterion), and an edge-count window — a child on k + 1 vertices with
 * m' edges is kept only when m' <= 3n - 6 and m' plus the maximum number
 * of edges the remaining n - k - 1 vertices can still add (vertex j can
 * add at most j - 1 edges) reaches 3n - 6. Both tests depend only on the
 * child's isomorphism class (planarity and the edge count are
 * invariants), and the canonical-deletion chain of every triangulation
 * stays inside the window (the deleted vertex at level j + 1 has degree
 * at most j), so McKay's one-parent-per-class argument is unaffected.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic maximal planar graphs on n >= 3 vertices =
 * number of planar triangulations = OEIS A000109(n): 1, 1, 1, 2, 5, 14,
 * 50, 233, 1249, ... for n = 3, 4, ...; K1 and K2 make the count 1 for
 * n = 1, 2 as well. Every output is connected (a planar graph with
 * 3n - 6 edges cannot have isolated parts), so there is no
 * `connected_only` flag.
 *
 * The dedicated generator for this class is plantri's canonical
 * construction path over the plane triangulations themselves (vertex
 * splittings on embedded triangulations, millions of graphs per second);
 * this implementation instead reuses the shared canonical-augmentation
 * machinery, whose per-child exact canonicalization keeps it practical
 * only to about n = 10.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Brinkmann, McKay, "Fast generation of planar graphs," MATCH Commun.
 *   Math. Comput. Chem. 58, 2007 (plantri; the dedicated algorithm);
 *   Brandes, "The left-right planarity test," 2009 (the recognizer);
 *   OEIS A000109
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
 * @brief Algorithm selection for non-isomorphic maximal planar enumeration
 */
enum class MaximalPlanarUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated maximal planar graph
 */
struct MaximalPlanarUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic maximal planar enumeration
 */
struct MaximalPlanarUnlabeledEnumerationResult {
    std::vector<MaximalPlanarUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline MaximalPlanarUnlabeledEnumeratedGraph maximal_planar_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    MaximalPlanarUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param edge_count Number of edges of the current graph
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param target_edges Edge count of an n-vertex maximal planar graph
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over the subsets of
 * the current vertex set whose size lies in the edge-count window (see
 * the file comment), keeping those for which the extended graph is still
 * planar. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. At the last level only graphs
 * reaching @p target_edges are emitted (the window forces the degree of
 * the final vertex to close the gap exactly, so the check is a formality).
 */
inline void maximal_planar_unlabeled_enum_dfs(
    int k, int edge_count, std::vector<unsigned long long>& adj, int n,
    int target_edges,
    std::vector<MaximalPlanarUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (edge_count == target_edges) {
            results.push_back(maximal_planar_unlabeled_build_graph(k, adj));
        }
        return;
    }

    // Edge-count window for the new vertex's degree. Vertices still to be
    // added after this one are k + 2, ..., n (1-indexed sizes), and vertex
    // j can contribute at most j - 1 edges.
    long long remaining_after = 0;
    for (int j = k + 2; j <= n; ++j) remaining_after += j - 1;
    long long d_min_ll = target_edges - edge_count - remaining_after;
    int d_min = d_min_ll > 0 ? static_cast<int>(d_min_ll) : 0;
    int d_max = target_edges - edge_count;
    if (d_max > k) d_max = k;
    if (d_min > d_max) return;

    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        int deg = 0;
        for (unsigned long long t = s; t; t &= t - 1) ++deg;
        if (deg < d_min || deg > d_max) continue;

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
                maximal_planar_unlabeled_enum_dfs(k + 1, edge_count + deg,
                                                  adj, n, target_edges,
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
 * @brief Enumerates all non-isomorphic maximal planar graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return MaximalPlanarUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: the single graphs K1,
 * K2 for n = 1, 2 and the A000109(n) planar triangulations
 * (1, 1, 1, 2, 5, 14, 50, 233, ... for n = 3, 4, ...) from there. n = 0
 * yields the single empty graph; negative n and n >= 64 yield nothing.
 * Every emitted graph is connected, so no `connected_only` flag exists.
 *
 * @note The search runs over all unlabeled planar graphs inside the
 *       edge-count window, and every candidate child costs one linear-time
 *       planarity recognition plus, when it is planar, one exact
 *       canonicalization (branch and bound over vertex orderings, worst
 *       case k! on vertex-transitive graphs), so the enumeration is
 *       practical to about n = 10.
 */
inline MaximalPlanarUnlabeledEnumerationResult
enumerate_maximal_planar_unlabeled_graphs(
    int n,
    MaximalPlanarUnlabeledEnumAlgorithm algo =
        MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    MaximalPlanarUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        MaximalPlanarUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    const int target_edges = n >= 3 ? 3 * n - 6 : n * (n - 1) / 2;
    std::vector<unsigned long long> adj(1, 0);
    detail::maximal_planar_unlabeled_enum_dfs(1, 0, adj, n, target_edges,
                                              result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
