#ifndef GRAPH_RECOGNITION_BICONNECTED_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_BICONNECTED_UNLABELED_ENUM_H

/**
 * @file biconnected_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic biconnected graphs
 *
 * Enumerates one representative per isomorphism class of biconnected
 * graphs (connected, n >= 3, no cut vertex) on n vertices by McKay's
 * canonical construction path method. The class is not hereditary —
 * deleting a vertex can disconnect the graph — so, unlike the hereditary
 * enumerators built on the same machinery, the intermediate levels must
 * generate all graphs. What biconnectivity does give is a constraint on
 * the last two levels (geng -C style): in a biconnected graph G the graph
 * G - v is connected for every vertex v, so only connected graphs are
 * generated at level n - 1, and the new last vertex must have degree at
 * least 2 with every vertex of degree below 2 among its neighbors (the
 * final minimum degree is 2). Graphs reaching level n still get a full
 * cut-vertex test at emission — the pruning is necessary, not sufficient.
 *
 * Isomorph rejection is the canonical-parent test at every level: one
 * canonicalization of each candidate child yields both its canonical form
 * and the automorphism orbit of the vertex placed last by the canonical
 * labeling, and the child survives only when the newly added vertex lies
 * in that orbit (so each class accepts exactly one parent class), with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`. The
 * level-(n-1) pruning respects the parent rule: the canonical parent of a
 * biconnected G is G minus a canonical-orbit vertex, which is connected,
 * and G arises from it by joining the new vertex to a set that satisfies
 * both degree conditions.
 *
 * Number of non-isomorphic biconnected graphs on n vertices =
 * OEIS A002218(n): 1, 3, 10, 56, 468, 7123, 194066, ... for n = 3, 4, ...
 * (0 for n < 3, matching the recognizer's size requirement).
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path); McKay, Piperno, "Practical graph
 *   isomorphism, II," J. Symbolic Comput. 60, 2014 (nauty/geng, whose
 *   `-C` option generates biconnected graphs the same way);
 *   OEIS A002218
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/biconnected.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic biconnected enumeration
 */
enum class BiconnectedUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path, connectivity-constrained last levels */
};

/**
 * @brief An enumerated biconnected graph
 */
struct BiconnectedUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic biconnected enumeration
 */
struct BiconnectedUnlabeledEnumerationResult {
    std::vector<BiconnectedUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Number of set bits */
inline int biconnected_unlabeled_popcount(unsigned long long b) {
    int c = 0;
    for (; b != 0; b &= b - 1) ++c;
    return c;
}

inline void biconnected_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<BiconnectedUnlabeledEnumeratedGraph>& results);

/**
 * @brief Extends the graph by a new vertex with neighborhood s and recurses
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current graph
 * @param s Neighborhood of the new vertex as a bitmask over 0, ..., k-1
 * @param n Target number of vertices
 * @param child_forms Canonical forms already produced by a sibling
 * @param results Storage for results
 *
 * The child survives the canonical-parent test when the new vertex lies in
 * its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument. A disconnected child at
 * level n - 1 is dropped before the canonicalization: it is neither
 * emitted nor the canonical parent of any biconnected graph (G - v is
 * connected for every v of a biconnected G).
 */
inline void biconnected_unlabeled_enum_extend(
    int k, std::vector<unsigned long long>& adj, unsigned long long s,
    int n, std::set<std::vector<unsigned long long> >& child_forms,
    std::vector<BiconnectedUnlabeledEnumeratedGraph>& results) {
    adj.push_back(s);
    for (int u = 0; u < k; ++u) {
        if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
    }

    if (k + 1 != n - 1 || bitmask_graph_connected(k + 1, adj)) {
        CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(k + 1, adj);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            biconnected_unlabeled_enum_dfs(k + 1, adj, n, results);
        }
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
 * @param results Storage for results
 *
 * Below level n - 1 the new vertex's neighborhood runs over all 2^k
 * subsets (biconnected graphs are not hereditary, so every graph is a
 * potential ancestor and no class pruning applies). At the last level the
 * current graph is connected by construction and the neighborhood is
 * restricted to the necessary conditions for biconnectivity: the new
 * vertex gets degree at least 2, and every vertex whose degree is still
 * below the final minimum degree 2 must be one of its neighbors. These
 * conditions hold for s = N_G(v) whenever G is biconnected and v is the
 * deleted canonical-orbit vertex (deleting v lowers only its neighbors'
 * degrees), so no biconnected class is lost. A graph reaching level n
 * gets the full cut-vertex test before emission.
 */
inline void biconnected_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<BiconnectedUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        BiconnectedUnlabeledEnumeratedGraph g;
        g.n = k;
        g.edges = bitmask_graph_edges(k, adj);
        Graph graph(k, g.edges);
        if (check_biconnected(graph).is_biconnected) {
            results.push_back(g);
        }
        return;
    }
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    if (k + 1 == n) {
        unsigned long long need = 0;
        for (int u = 0; u < k; ++u) {
            if (biconnected_unlabeled_popcount(adj[u]) < 2) need |= 1ULL << u;
        }
        for (unsigned long long s = 0; s < limit; ++s) {
            if ((s & need) != need) continue;
            if (biconnected_unlabeled_popcount(s) < 2) continue;
            biconnected_unlabeled_enum_extend(k, adj, s, n, child_forms,
                                              results);
        }
    } else {
        for (unsigned long long s = 0; s < limit; ++s) {
            biconnected_unlabeled_enum_extend(k, adj, s, n, child_forms,
                                              results);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic biconnected graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return BiconnectedUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A002218(n) graphs
 * (1, 3, 10, 56, 468, 7123, ... for n = 3, 4, ...). n < 3 yields nothing —
 * the recognizer requires at least 3 vertices, matching the labeled
 * enumerator — and so do negative n and n >= 64. Biconnected graphs are
 * connected by definition, so there is no `connected_only` flag.
 *
 * @note The intermediate levels enumerate every unlabeled graph on up to
 *       n - 2 vertices (A000088) plus the connected ones on n - 1, and the
 *       last level branches over up to 2^(n-1) candidate neighborhoods per
 *       parent with one exact branch-and-bound canonicalization apiece, so
 *       the enumeration is practical to about n = 9
 *       (n = 8 takes well under a second, n = 9 about 20 seconds;
 *       A002218(9) = 194066).
 */
inline BiconnectedUnlabeledEnumerationResult
enumerate_biconnected_unlabeled_graphs(
    int n,
    BiconnectedUnlabeledEnumAlgorithm algo =
        BiconnectedUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    BiconnectedUnlabeledEnumerationResult result;
    if (n < 3 || n >= 64) return result;
    std::vector<unsigned long long> adj(1, 0);
    detail::biconnected_unlabeled_enum_dfs(1, adj, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
