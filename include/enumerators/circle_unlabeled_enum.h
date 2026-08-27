#ifndef GRAPH_RECOGNITION_CIRCLE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CIRCLE_UNLABELED_ENUM_H

/**
 * @file circle_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic circle graphs
 *
 * Enumerates one representative per isomorphism class of circle graphs on
 * n vertices by McKay's canonical construction path method, the scheme
 * Johnston (2020) calls canonical deletion and applies to permutation and
 * circle graphs (computing the circle counts up to n = 13). Graphs are
 * grown one vertex at a time, which is sound because circle graphs are
 * hereditary: deleting the canonically last vertex of a circle graph again
 * yields a circle graph, so every class is reachable from the one-vertex
 * graph.
 *
 * As with the permutation enumerator, no cheap incremental test decides
 * whether adding a vertex keeps the graph in the class, so the pruning is
 * a recognizer call (`check_circle` with the default polynomial-time
 * NAJI_SYSTEM algorithm) on every candidate child. It is applied before
 * the isomorph rejection, so the (more expensive) canonicalization only
 * ever runs on circle graphs.
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic circle graphs on n vertices =
 * OEIS A156809(n): 1, 2, 4, 11, 34, 154, 978, 9497, 127954, ...
 * (every graph on at most 5 vertices is a circle graph, so the first five
 * terms agree with A000088). The connected ones are
 * OEIS A156808(n): 1, 1, 2, 6, 21, 110, 789, 8336, 117283, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Johnston, canonical-deletion enumeration of permutation / circle
 *   graphs, 2020;
 *   Naji, "Reconnaissance des graphes de cordes," Discrete Math. 54, 1985;
 *   OEIS A156809, A156808
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/circle.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic circle enumeration
 */
enum class CircleUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated circle graph
 */
struct CircleUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic circle enumeration
 */
struct CircleUnlabeledEnumerationResult {
    std::vector<CircleUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline CircleUnlabeledEnumeratedGraph circle_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    CircleUnlabeledEnumeratedGraph g;
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
 * current vertex set, keeping those for which the extended graph is still a
 * circle graph. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void circle_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<CircleUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(circle_unlabeled_build_graph(k, adj));
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
        if (check_circle(child).is_circle) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                circle_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic circle graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return CircleUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A156809(n) graphs
 * (1, 2, 4, 11, 34, 154, 978, 9497, ... for n = 1, 2, ...), or the
 * connected ones among them (1, 1, 2, 6, 21, 110, 789, 8336, ...) with
 * @p connected_only. n = 0 yields the single empty graph in both modes;
 * negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one circle recognition (Naji's GF(2)
 *       system) plus, when it is a circle graph, one exact canonicalization
 *       (branch and bound over vertex orderings, worst case k! on
 *       vertex-transitive graphs), so the enumeration is practical to about
 *       n = 9 (n = 8 takes about 2 seconds, n = 9 about 50 seconds;
 *       A156809(9) = 127954).
 */
inline CircleUnlabeledEnumerationResult
enumerate_circle_unlabeled_graphs(
    int n, bool connected_only = false,
    CircleUnlabeledEnumAlgorithm algo =
        CircleUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    CircleUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        CircleUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::circle_unlabeled_enum_dfs(1, adj, n, connected_only,
                                      result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
