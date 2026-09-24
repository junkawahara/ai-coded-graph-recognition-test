#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_UNLABELED_ENUM_H

/**
 * @file maximal_planar_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic maximal planar graphs (triangulations)
 *
 * PLANTRI (default) uses Brinkmann--McKay's E3/E4/E5 expansions from K4.
 * A rotation system preserves the triangular faces. Rooted directed-edge
 * BFS in both orientations gives canonical embeddings, and a canonical
 * inverse expansion gives one parent class per child. Sibling canonical
 * codes remove duplicate expansions. No planarity recognizer is used.
 * For these 3-connected graphs, embedding isomorphism including reflection
 * agrees with abstract graph isomorphism.
 *
 * CANONICAL_AUGMENTATION retains the previous search through all planar
 * vertex additions, pruning by planarity and an edge-count window and
 * using the generic canonical vertex-deletion orbit. Its output order
 * and representative labels are unchanged.
 *
 * Counts for n = 3, 4, ... are 1, 1, 1, 2, 5, 14, 50, 233, 1249, ...
 * (OEIS A000109). n = 0, 1, 2 each yields K_n. Every nonempty output is
 * connected, so there is no connected_only option.
 *
 * The new implementation copies rotations and stores sibling codes; it
 * does not reproduce plantri's in-place updates or expansion-orbit pruning.
 * See util/maximal_planar_expansion.h for costs. All outputs are retained.
 *
 * References: Brinkmann--McKay, Fast generation of planar graphs, MATCH
 * 58 (2007), sections 1.2--1.3; McKay, Isomorph-free exhaustive generation,
 * J. Algorithms 26 (1998).
 */

#include <cstddef>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

#include "recognizers/planar.h"
#include "util/canonical_augmentation.h"
#include "util/graph.h"
#include "util/maximal_planar_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic maximal planar enumeration
 */
enum class MaximalPlanarUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION = 0, /**< Legacy planar vertex additions */
    PLANTRI = 1 /**< E3/E4/E5 triangulation expansions and canonical reductions (default) */
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
 * @param n Number of vertices (0..63, preserving the legacy mask limit)
 * @param algo Algorithm to use (default: PLANTRI)
 * @return One graph per isomorphism class, retaining all output in memory
 *
 * PLANTRI has O(k) candidate patches per k-vertex parent and O(k^2)
 * canonicalization work per candidate. The DFS path and sibling sets take
 * O(n^3) space, separately from the output. CANONICAL_AUGMENTATION retains
 * the legacy planarity checks and worst-case factorial canonicalization.
 * @throws std::invalid_argument if n < 0, n > 63, or algo is unknown
 */
inline MaximalPlanarUnlabeledEnumerationResult
enumerate_maximal_planar_unlabeled_graphs(
    int n,
    MaximalPlanarUnlabeledEnumAlgorithm algo =
        MaximalPlanarUnlabeledEnumAlgorithm::PLANTRI) {
    MaximalPlanarUnlabeledEnumerationResult result;
    require_vertex_count("enumerate_maximal_planar_unlabeled_graphs", n, 63);
    if (algo == MaximalPlanarUnlabeledEnumAlgorithm::PLANTRI) {
        detail::maximal_planar_expansion_enumerate(n,
            [&](const std::vector<std::pair<int, int> >& edges) {
                MaximalPlanarUnlabeledEnumeratedGraph graph;
                graph.n = n;
                graph.edges = edges;
                result.graphs.push_back(std::move(graph));
            });
        return result;
    }
    if (algo != MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION)
        throw std::invalid_argument("unknown maximal planar unlabeled enumeration algorithm");
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
