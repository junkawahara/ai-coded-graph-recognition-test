#ifndef GRAPH_RECOGNITION_DEGENERATE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_DEGENERATE_UNLABELED_ENUM_H

/**
 * @file degenerate_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic k-degenerate graphs
 *
 * Enumerates one representative per isomorphism class of the
 * k-degenerate graphs on n vertices by McKay's canonical construction
 * path method — the survey's "geng + degeneracy filter" route realized
 * on the shared canonical-augmentation machinery. Graphs are grown one
 * vertex at a time, which is sound because k-degeneracy is hereditary:
 * deleting the canonically last vertex of a k-degenerate graph again
 * yields a k-degenerate graph, so every class is reachable from the
 * one-vertex graph and the recognizer (`check_degenerate`, the O(n + m)
 * minimum-degree peeling) can prune every candidate child before the
 * far more expensive canonicalization runs.
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
 * ...; connected = trees, A000055), k = 0 yields only the edgeless
 * graph, and k >= n-1 yields all graphs (A000088: 1, 2, 4, 11, 34, 156,
 * ...). The 2-degenerate counts (1, 2, 4, 10, 28, 105, 508, 3454,
 * 31935 for n = 1..9), the connected 2-degenerate counts (1, 1, 2, 5,
 * 16, 68, 375, 2822, ...) and the 3-degenerate counts (1, 2, 4, 11,
 * 33, 148, 950, ... for n = 1..7) are not in the OEIS. Since
 * treewidth <= k implies k-degeneracy, the output is a superset of
 * `enumerate_partial_ktree_unlabeled_graphs(n, k)`, proper from
 * n = 5, k = 2 on (28 vs 27 classes: K4 with one edge subdivided is
 * 2-degenerate but has a K4 minor, hence treewidth 3).
 *
 * References:
 *   Bauer, Krug, Wagner, "Enumerating and generating labeled
 *   k-degenerate graphs," ANALCO 2010 (the survey's dedicated *labeled*
 *   counting/generation via well-orderings; not used by this
 *   non-isomorphic route);
 *   Lick, White, "k-degenerate graphs," Canad. J. Math. 22, 1970;
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   OEIS A005195, A000055, A000088
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/degenerate.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic k-degenerate enumeration
 */
enum class DegenerateUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated k-degenerate graph
 */
struct DegenerateUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic k-degenerate enumeration
 */
struct DegenerateUnlabeledEnumerationResult {
    std::vector<DegenerateUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline DegenerateUnlabeledEnumeratedGraph degenerate_unlabeled_build_graph(
    int c, const std::vector<unsigned long long>& adj) {
    DegenerateUnlabeledEnumeratedGraph g;
    g.n = c;
    g.edges = bitmask_graph_edges(c, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a c-vertex canonical representative
 * @param c Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param k Degeneracy bound
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over all subsets of
 * the current vertex set, keeping those for which the extended graph is
 * still k-degenerate. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class
 * is McKay's canonical construction path argument. Connectivity cannot
 * be pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void degenerate_unlabeled_enum_dfs(
    int c, std::vector<unsigned long long>& adj, int n, int k,
    bool connected_only,
    std::vector<DegenerateUnlabeledEnumeratedGraph>& results) {
    if (c == n) {
        if (!connected_only || bitmask_graph_connected(c, adj)) {
            results.push_back(degenerate_unlabeled_build_graph(c, adj));
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
        if (check_degenerate(child, k).is_degenerate) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(c + 1, adj);
            if (((canon.last_orbit >> c) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                degenerate_unlabeled_enum_dfs(c + 1, adj, n, k,
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
 * @brief Enumerates all non-isomorphic k-degenerate graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param k Degeneracy bound (the class parameter)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return DegenerateUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of the k-degenerate
 * graphs. k = 0 yields only the edgeless graph (none in connected mode
 * for n >= 2); k >= n-1 yields all graphs (A000088). n = 0 yields the
 * single empty graph in both modes (it is k-degenerate for every k);
 * negative n, n >= 64 and negative k yield nothing.
 *
 * @note Every candidate child costs one O(n + m) peeling plus, when it
 *       passes, one exact canonicalization (branch and bound over vertex
 *       orderings, worst case c! on vertex-transitive graphs), so the
 *       enumeration is practical to about n = 8 (n = 9 in minutes for
 *       small k).
 */
inline DegenerateUnlabeledEnumerationResult
enumerate_degenerate_unlabeled_graphs(
    int n, int k, bool connected_only = false,
    DegenerateUnlabeledEnumAlgorithm algo =
        DegenerateUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    DegenerateUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64 || k < 0) return result;
    if (n == 0) {
        DegenerateUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::degenerate_unlabeled_enum_dfs(1, adj, n, k, connected_only,
                                          result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
