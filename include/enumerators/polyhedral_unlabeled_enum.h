#ifndef GRAPH_RECOGNITION_POLYHEDRAL_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_UNLABELED_ENUM_H

/**
 * @file polyhedral_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic polyhedral graphs (3-connected planar)
 *
 * PLANTRI (default) shares the embedded E3/E4/E5 triangulation generator
 * with maximal planar enumeration. At fixed n it deletes edges incident
 * with a triangle, checking the merged face's intersections with the other
 * faces to preserve 3-connectivity. The first missing ear diagonal in the
 * canonical embedding selects each child's parent. Mirror images and
 * sibling duplicates are identified, without a global visited set or
 * planarity/triconnectivity recognition during generation.
 *
 * CANONICAL_AUGMENTATION retains the original McKay vertex-addition search
 * over planar graphs: planarity and an edge-count window prune candidates,
 * with full polyhedral recognition at n vertices. The window is
 * ceil(3n/2) <= m <= 3n-6 at the leaves, allowing the remaining vertices
 * to contribute at most the sum of their possible degrees at earlier levels.
 * These bounds depend only on the child's isomorphism class and retain
 * every canonical vertex-deletion chain. Output order and labels are unchanged.
 *
 * Counts (OEIS A000944): 1, 2, 7, 34, 257, 2606, 32300 for n = 4..10;
 * n < 4 yields nothing. There is no connected_only flag: every output is
 * 3-connected. All results are retained in memory.
 *
 * References: Brinkmann--McKay, "Fast generation of planar graphs,"
 * MATCH 58 (2007), sections 1.2--1.3 and 4; McKay, "Isomorph-free
 * exhaustive generation," J. Algorithms 26 (1998). The implementation
 * does not depend on the external plantri program or claim its throughput.
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "recognizers/planar.h"
#include "recognizers/polyhedral.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"
#include "util/vertex_count.h"
#include "util/polyhedral_edge_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic polyhedral enumeration
 */
enum class PolyhedralUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION, /**< Legacy planar vertex additions with canonical deletion */
    PLANTRI /**< Canonical edge deletions from embedded triangulations */
};

/**
 * @brief An enumerated polyhedral graph
 */
struct PolyhedralUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic polyhedral enumeration
 */
struct PolyhedralUnlabeledEnumerationResult {
    std::vector<PolyhedralUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline PolyhedralUnlabeledEnumeratedGraph polyhedral_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    PolyhedralUnlabeledEnumeratedGraph g;
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
 * @param min_edges Minimum edge count of an n-vertex polyhedral graph
 * @param max_edges Maximum edge count of an n-vertex polyhedral graph
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over the subsets of
 * the current vertex set whose size lies in the edge-count window (see
 * the file comment), keeping those for which the extended graph is still
 * planar. A child survives when the new vertex lies in its
 * canonical-deletion orbit and its canonical form was not already produced
 * by a sibling; correctness of accepting one parent per class is McKay's
 * canonical construction path argument. 3-connectivity is not hereditary,
 * so at the last level the full `check_polyhedral` decides emission.
 */
inline void polyhedral_unlabeled_enum_dfs(
    int k, int edge_count, std::vector<unsigned long long>& adj, int n,
    int min_edges, int max_edges,
    std::vector<PolyhedralUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        Graph g(k, bitmask_graph_edges(k, adj));
        if (check_polyhedral(g).is_polyhedral) {
            results.push_back(polyhedral_unlabeled_build_graph(k, adj));
        }
        return;
    }

    // Edge-count window for the new vertex's degree. Vertices still to be
    // added after this one are k + 2, ..., n (1-indexed sizes), and vertex
    // j can contribute at most j - 1 edges.
    long long remaining_after = 0;
    for (int j = k + 2; j <= n; ++j) remaining_after += j - 1;
    long long d_min_ll = min_edges - edge_count - remaining_after;
    int d_min = d_min_ll > 0 ? static_cast<int>(d_min_ll) : 0;
    int d_max = max_edges - edge_count;
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
                polyhedral_unlabeled_enum_dfs(k + 1, edge_count + deg, adj,
                                              n, min_edges, max_edges,
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
 * @brief Enumerates all non-isomorphic polyhedral graphs on n vertices
 * @param n Number of vertices (0..63)
 * @param algo Algorithm to use (default: PLANTRI)
 * @return PolyhedralUnlabeledEnumerationResult; empty for n < 4
 *
 * PLANTRI uses O(n) edge candidates per output and O(n^2) work per
 * candidate, with O(n^3) search storage, excluding collected output.
 * Canonical ear additions lead to one of the n-vertex triangulation roots;
 * each class has exactly one parent class and is emitted once. There is
 * no connected_only flag. CANONICAL_AUGMENTATION keeps the original
 * planar vertex-addition algorithm and its output order and labels.
 * @throws std::invalid_argument if n < 0, n > 63, or algo is invalid
 *         (the legacy neighbourhoods are 64-bit masks;
 *          the same API limit is retained for PLANTRI)
 */
inline PolyhedralUnlabeledEnumerationResult
enumerate_polyhedral_unlabeled_graphs(
    int n,
    PolyhedralUnlabeledEnumAlgorithm algo =
        PolyhedralUnlabeledEnumAlgorithm::PLANTRI) {
    PolyhedralUnlabeledEnumerationResult result;
    require_vertex_count("enumerate_polyhedral_unlabeled_graphs", n, 63);
    if (algo != PolyhedralUnlabeledEnumAlgorithm::PLANTRI &&
        algo != PolyhedralUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION)
        throw std::invalid_argument("invalid polyhedral unlabeled enumeration algorithm");
    if (n < 4) return result;
    if (algo == PolyhedralUnlabeledEnumAlgorithm::PLANTRI) {
        detail::polyhedral_edge_enumerate(n, [&](const std::vector<std::pair<int, int>>& edges) {
            PolyhedralUnlabeledEnumeratedGraph g;
            g.n = n;
            g.edges = edges;
            result.graphs.push_back(std::move(g));
        });
        return result;
    }
    const int min_edges = (3 * n + 1) / 2;
    const int max_edges = 3 * n - 6;
    std::vector<unsigned long long> adj(1, 0);
    detail::polyhedral_unlabeled_enum_dfs(1, 0, adj, n, min_edges, max_edges,
                                          result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
