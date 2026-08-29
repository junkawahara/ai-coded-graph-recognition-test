#ifndef GRAPH_RECOGNITION_CACTUS_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CACTUS_UNLABELED_ENUM_H

/**
 * @file cactus_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic cactus graphs
 *
 * Enumerates one representative per isomorphism class of cactus graphs
 * (every biconnected component a single edge or a simple cycle) on n
 * vertices by McKay's canonical construction path method. Graphs are
 * grown one vertex at a time, which is sound because cactus graphs are
 * hereditary: deleting the canonically last vertex only shrinks blocks
 * (a cycle block loses at most one vertex and falls apart into edges),
 * so every class is reachable from the one-vertex graph.
 *
 * The pruning is a recognizer call (`check_cactus`, a DFS biconnected
 * component decomposition) on every candidate child, applied before the
 * isomorph rejection so the far more expensive canonicalization only
 * ever runs on cactus graphs. (The dedicated enumerator of Bahrani and
 * Lumbroso derives the class's split-decomposition grammar and generates
 * the non-isomorphic cacti directly from it, with an O(1)-per-graph
 * rooted version; this implementation instead reuses the shared
 * canonical-augmentation machinery, which is exact and practical to
 * about n = 10, so that survey's per-graph bound does not apply here.)
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
 *   Bahrani, Lumbroso, "Enumerations, forbidden subgraph
 *   characterizations, and the split-decomposition," Electron. J. Combin.
 *   25(4), 2018 (the dedicated split-decomposition-grammar enumerator;
 *   also arXiv:1711.10647)
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/cactus.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic cactus enumeration
 */
enum class CactusUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (canonical deletion) */
};

/**
 * @brief An enumerated cactus graph
 */
struct CactusUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic cactus enumeration
 */
struct CactusUnlabeledEnumerationResult {
    std::vector<CactusUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline CactusUnlabeledEnumeratedGraph cactus_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    CactusUnlabeledEnumeratedGraph g;
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
 * a cactus. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void cactus_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<CactusUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(cactus_unlabeled_build_graph(k, adj));
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
        if (check_cactus(child).is_cactus) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                cactus_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic cactus graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return CactusUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class of cactus graphs
 * (1, 2, 4, 9, 20, 51, 133, 380, 1144, ... for n = 1, 2, ...; not in the
 * OEIS), or the connected ones among them (A000083) with
 * @p connected_only. n = 0 yields the single empty graph in both modes;
 * negative n and n >= 64 yield nothing.
 *
 * @note Every candidate child costs one cactus recognition (a DFS
 *       biconnected component decomposition) plus, when it passes, one
 *       exact canonicalization (branch and bound over vertex orderings,
 *       worst case k! on vertex-transitive graphs such as the cycles), so
 *       the enumeration is practical to about n = 10.
 */
inline CactusUnlabeledEnumerationResult enumerate_cactus_unlabeled_graphs(
    int n, bool connected_only = false,
    CactusUnlabeledEnumAlgorithm algo =
        CactusUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    CactusUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        CactusUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::cactus_unlabeled_enum_dfs(1, adj, n, connected_only,
                                      result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
