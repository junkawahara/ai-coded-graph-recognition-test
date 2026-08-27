#ifndef GRAPH_RECOGNITION_EULERIAN_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_EULERIAN_UNLABELED_ENUM_H

/**
 * @file eulerian_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Eulerian graphs
 *
 * Enumerates one representative per isomorphism class of Eulerian graphs
 * (all degrees even) on n vertices by McKay's canonical construction path
 * method. The class is not hereditary — deleting a vertex of positive
 * degree breaks the parity of its neighbors — so, unlike the hereditary
 * enumerators built on the same machinery, the intermediate levels must
 * generate all graphs. The even-degree constraint instead collapses the
 * last level: in an Eulerian graph G the odd-degree vertices of G - v are
 * exactly N_G(v), so every (n-1)-vertex graph H extends to an Eulerian
 * graph in exactly one way (join the new vertex to the odd-degree vertices
 * of H, an even-sized set by the handshake lemma), and the search branches
 * over the 2^k neighborhoods only up to level n - 1.
 *
 * Isomorph rejection is the canonical-parent test at every level: one
 * canonicalization of each candidate child yields both its canonical form
 * and the automorphism orbit of the vertex placed last by the canonical
 * labeling, and the child survives only when the newly added vertex lies
 * in that orbit (so each class accepts exactly one parent class), with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`. On the
 * forced last level the argument is unchanged: the canonical parent of an
 * Eulerian graph G is G minus a canonical-orbit vertex, an arbitrary
 * (n-1)-vertex graph, and its unique extension reconstructs G.
 *
 * Number of non-isomorphic Eulerian graphs on n vertices =
 * OEIS A002854(n): 1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120, ...
 * The connected ones are counted by A003049: 1, 0, 1, 1, 4, 8, 37, 184, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path); Harary, Palmer, "Graphical
 *   Enumeration," Academic Press, 1973 (even-graph counting; the deletion
 *   bijection behind the labeled count 2^((n-1)(n-2)/2));
 *   OEIS A002854, A003049
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic Eulerian enumeration
 */
enum class EulerianUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, forced last level */
};

/**
 * @brief An enumerated Eulerian graph
 */
struct EulerianUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic Eulerian enumeration
 */
struct EulerianUnlabeledEnumerationResult {
    std::vector<EulerianUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline EulerianUnlabeledEnumeratedGraph eulerian_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    EulerianUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

inline void eulerian_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<EulerianUnlabeledEnumeratedGraph>& results);

/**
 * @brief Extends the graph by a new vertex with neighborhood s and recurses
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., k-1
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument.
 */
inline void eulerian_unlabeled_enum_extend(
    int k, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, bool connected_only,
    std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<EulerianUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
    }

    CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(k + 1, adj);
    if (((canon.last_orbit >> k) & 1ULL) &&
        child_forms.insert(canon.form).second) {
        eulerian_unlabeled_enum_dfs(k + 1, adj, n, connected_only, results);
    }

    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
    }
    adj.pop_back();
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param n Target number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param results Storage for results
 *
 * Below level n - 1 the new vertex's neighborhood runs over all 2^k
 * subsets (Eulerian graphs are not hereditary, so every graph is a
 * potential ancestor and no class pruning applies); at level n - 1 it is
 * forced to the odd-degree vertices, the unique completion to all-even
 * degrees. A graph reaching level n is therefore Eulerian by construction.
 * Connectivity cannot be pruned during the search (later vertices may join
 * components), so `connected_only` filters at emission.
 */
inline void eulerian_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<EulerianUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(eulerian_unlabeled_build_graph(k, adj));
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    if (k + 1 == n) {
        unsigned long long odd = 0;
        for (int u = 0; u < k; ++u) {
            int deg = 0;
            for (unsigned long long b = adj[u]; b != 0; b &= b - 1) ++deg;
            if (deg % 2 == 1) odd |= 1ULL << u;
        }
        eulerian_unlabeled_enum_extend(k, adj, odd, n, connected_only,
                                       child_forms, results);
    } else {
        const unsigned long long limit = 1ULL << k;
        for (unsigned long long s = 0; s < limit; ++s) {
            eulerian_unlabeled_enum_extend(k, adj, s, n, connected_only,
                                           child_forms, results);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic Eulerian graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return EulerianUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A002854(n) graphs
 * (1, 1, 2, 3, 7, 16, 54, 243, ... for n = 1, 2, ...), or A003049(n)
 * (1, 0, 1, 1, 4, 8, 37, 184, ...) with @p connected_only. n = 0 yields
 * the single empty graph in both modes; negative n and n >= 64 yield
 * nothing.
 *
 * @note The intermediate levels enumerate every unlabeled graph on up to
 *       n - 1 vertices (A000088), each branching over 2^k candidate
 *       neighborhoods with one exact branch-and-bound canonicalization
 *       apiece, so the enumeration is practical to about n = 10
 *       (n = 9 takes about a second, n = 10 about 40 seconds;
 *       A002854(10) = 33120).
 */
inline EulerianUnlabeledEnumerationResult
enumerate_eulerian_unlabeled_graphs(
    int n, bool connected_only = false,
    EulerianUnlabeledEnumAlgorithm algo =
        EulerianUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    EulerianUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        EulerianUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::eulerian_unlabeled_enum_dfs(1, adj, n, connected_only,
                                        result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
