#ifndef GRAPH_RECOGNITION_SERIES_PARALLEL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_SERIES_PARALLEL_UNLABELED_ENUM_H

/**
 * @file series_parallel_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic series-parallel graphs
 *
 * Enumerates one representative per isomorphism class of series-parallel
 * (K4-minor-free) graphs on n vertices by McKay's canonical construction
 * path method. Graphs are grown one vertex at a time, which is sound
 * because series-parallel graphs are hereditary (minor-closed): deleting
 * the canonically last vertex of a series-parallel graph again yields a
 * series-parallel graph, so every class is reachable from the one-vertex
 * graph.
 *
 * The pruning is a recognizer call (`check_series_parallel`, the
 * queue-based series/parallel reduction) on every candidate child, applied
 * before the isomorph rejection so the far more expensive canonicalization
 * only ever runs on series-parallel graphs. (The dedicated enumerator of
 * Kawano and Nakano generates rooted connected series-parallel graphs in
 * O(1) amortized time per graph by growing the decomposition tree directly
 * and never canonicalizes; this implementation instead reuses the shared
 * canonical-augmentation machinery, which is exact and practical to about
 * n = 10, so that survey's per-graph bound does not apply here.)
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Duffin, "Topology of series-parallel networks," J. Math. Anal. Appl.
 *   10, 1965 (series-parallel = K4-minor-free, behind the recognizer's
 *   reduction criterion);
 *   Kawano, Nakano, "Constant time generation of series-parallel graphs,"
 *   IEICE Trans. Fundamentals E88-A, 2005 (the dedicated O(1)-amortized
 *   rooted enumerator)
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/series_parallel.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic series-parallel enumeration
 */
enum class SeriesParallelUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated series-parallel graph
 */
struct SeriesParallelUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic series-parallel enumeration
 */
struct SeriesParallelUnlabeledEnumerationResult {
    std::vector<SeriesParallelUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline SeriesParallelUnlabeledEnumeratedGraph series_parallel_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    SeriesParallelUnlabeledEnumeratedGraph g;
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
 * series-parallel. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void series_parallel_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<SeriesParallelUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(series_parallel_unlabeled_build_graph(k, adj));
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
        if (check_series_parallel(child).is_series_parallel) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                series_parallel_unlabeled_enum_dfs(k + 1, adj, n,
                                                   connected_only, results);
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
 * @brief Enumerates all non-isomorphic series-parallel graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return SeriesParallelUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of K4-minor-free graphs,
 * or the connected ones among them with @p connected_only. n = 0 yields
 * the single empty graph in both modes; negative n and n >= 64 yield
 * nothing.
 *
 * @note Every candidate child costs one series-parallel recognition (a
 *       series/parallel reduction) plus, when it passes, one exact
 *       canonicalization (branch and bound over vertex orderings, worst
 *       case k! on vertex-transitive graphs such as the cycles), so the
 *       enumeration is practical to about n = 10.
 */
inline SeriesParallelUnlabeledEnumerationResult
enumerate_series_parallel_unlabeled_graphs(
    int n, bool connected_only = false,
    SeriesParallelUnlabeledEnumAlgorithm algo =
        SeriesParallelUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    SeriesParallelUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        SeriesParallelUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::series_parallel_unlabeled_enum_dfs(1, adj, n, connected_only,
                                               result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
