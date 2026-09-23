#ifndef GRAPH_RECOGNITION_PERMUTATION_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_PERMUTATION_UNLABELED_ENUM_H

/**
 * @file permutation_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic permutation graphs
 *
 * Enumerates one representative per isomorphism class of permutation graphs
 * on n vertices by Yamazaki et al.'s modular canonical edge-parent search.
 * The default starts at K_n and deletes edges, using modular decomposition
 * and the four prime quotient diagrams to canonicalize. A child is accepted
 * only once per parent and only if its first valid canonical edge addition
 * leads back to that parent. Disconnected graphs are included in this search;
 * connected_only safely prunes deletions that disconnect the graph.
 *
 * The legacy option uses McKay's canonical construction path method, the scheme
 * Johnston (2020) calls canonical deletion and applies to exactly this
 * class. Graphs are grown one vertex at a time, which is sound because
 * permutation graphs are hereditary: deleting the canonically last vertex of
 * a permutation graph again yields a permutation graph, so every class is
 * reachable from the one-vertex graph.
 *
 * In that legacy algorithm, no cheap incremental test
 * decides whether adding a vertex keeps the graph in the class, so the
 * pruning is a recognizer call (`check_permutation`, i.e. G and its
 * complement are both comparability graphs) on every candidate child. It is
 * applied before the isomorph rejection, so the (more expensive)
 * canonicalization only ever runs on graphs of the class.
 *
 * Its isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic permutation graphs on n vertices =
 * OEIS A123448(n): 1, 2, 4, 11, 33, 142, 776, 5699, 50723, ...
 * The connected ones are 1, 1, 2, 6, 20, 99, 600, 4753, 44068, ...
 *
 * References:
 *   Yamazaki, Saitoh, Kiyomi, Uehara, "Enumeration of nonisomorphic interval
 *   graphs and nonisomorphic permutation graphs," TCS 806 (2020), 310-322;
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Johnston, canonical-deletion enumeration of permutation graphs,
 *   2020;
 *   Pnueli, Lempel, Even, "Transitive orientation of graphs and
 *   identification of permutation graphs," Canad. J. Math. 23, 1971;
 *   OEIS A123448
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/permutation.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"
#include "util/permutation_edge_enum.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic permutation enumeration
 */
enum class PermutationUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION = 0, /**< Previous McKay canonical vertex additions */
    MODULAR_DECOMPOSITION = 1 /**< Modular canonical edge-parent search (default) */
};

/**
 * @brief An enumerated permutation graph
 */
struct PermutationUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic permutation enumeration
 */
struct PermutationUnlabeledEnumerationResult {
    std::vector<PermutationUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline PermutationUnlabeledEnumeratedGraph permutation_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    PermutationUnlabeledEnumeratedGraph g;
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
 * permutation graph. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. Connectivity cannot be pruned
 * during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void permutation_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<PermutationUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(permutation_unlabeled_build_graph(k, adj));
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
        if (check_permutation(child).is_permutation) {
            CanonicalAugmentationCanon canon =
                canonicalize_bitmask_graph(k + 1, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                permutation_unlabeled_enum_dfs(k + 1, adj, n, connected_only,
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
 * @brief Enumerates all non-isomorphic permutation graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: MODULAR_DECOMPOSITION)
 * @return PermutationUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A123448(n) graphs
 * (1, 2, 4, 11, 33, 142, 776, 5699, ... for n = 1, 2, ...), or the
 * connected ones among them (1, 1, 2, 6, 20, 99, 600, 4753, ...) with
 * @p connected_only. n = 0 yields the single empty graph in both modes;
 * negative n and n >= 64 throw std::invalid_argument.
 *
 * @note The default uses polynomial modular canonicalization, sibling sets,
 *       and local edge recognition. It uses the existing closure-based module
 *       decomposition and matrix recognizer rather than the paper's linear-time
 *       primitives, so the original delay bound is not claimed. All outputs
 *       are retained. CANONICAL_AUGMENTATION retains its previous representative
 *       labelings and order, including factorial worst-case canonicalization.
 * @throws std::invalid_argument if n < 0 or n > 63
 *         (legacy vertex neighbourhoods are 64-bit masks; the public limit is
 *          retained for both algorithms), or if algo is unknown
 */
inline PermutationUnlabeledEnumerationResult
enumerate_permutation_unlabeled_graphs(
    int n, bool connected_only = false,
    PermutationUnlabeledEnumAlgorithm algo =
        PermutationUnlabeledEnumAlgorithm::MODULAR_DECOMPOSITION) {
    PermutationUnlabeledEnumerationResult result;
    require_vertex_count("enumerate_permutation_unlabeled_graphs", n, 63);
    if (algo == PermutationUnlabeledEnumAlgorithm::MODULAR_DECOMPOSITION) {
        detail::enumerate_permutation_modular_edges(n, connected_only,
            [&](int size, const std::vector<std::pair<int, int>>& edges) {
                PermutationUnlabeledEnumeratedGraph g;
                g.n = size;
                g.edges = edges;
                result.graphs.push_back(std::move(g));
            });
        return result;
    }
    if (algo != PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION)
        throw std::invalid_argument("enumerate_permutation_unlabeled_graphs: unknown algorithm");
    if (n == 0) {
        PermutationUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::permutation_unlabeled_enum_dfs(1, adj, n, connected_only,
                                           result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
