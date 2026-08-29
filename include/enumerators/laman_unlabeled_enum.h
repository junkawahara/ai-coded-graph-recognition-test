#ifndef GRAPH_RECOGNITION_LAMAN_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_LAMAN_UNLABELED_ENUM_H

/**
 * @file laman_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Laman graphs
 *
 * Enumerates one representative per isomorphism class of Laman graphs
 * (minimally rigid graphs in 2D, (2,3)-tight graphs) on n vertices by
 * McKay's canonical construction path method, the same route as the
 * nauty-laman-plugin (geng growing through (2,3)-sparse graphs). Laman
 * graphs themselves are not closed under vertex deletion (the edge count
 * 2n - 3 breaks), but (2,3)-sparsity is hereditary, so the intermediate
 * levels range over (2,3)-sparse graphs and tightness (m = 2n - 3) filters
 * at emission. Every Laman graph is reachable because deleting the
 * canonically last vertex of a sparse graph again yields a sparse graph.
 *
 * Pruning on every candidate child (a new vertex with neighborhood S),
 * applied before the expensive canonicalization:
 *   1. sparsity edge bound: m > 2x - 3 on x vertices can never happen;
 *   2. tightness reachability: each future vertex j (0-indexed) adds at
 *      most j edges, so prune when m + sum_{j=x}^{n-1} j < 2n - 3
 *      (sound: deleting a vertex from a j+1-vertex graph removes at most
 *      j edges, so every canonical construction path satisfies the bound);
 *   3. degree feasibility: a final Laman graph on n >= 3 vertices has
 *      minimum degree 2 and each future vertex adds at most one edge per
 *      existing vertex, so prune when deg(v) + (n - x) < 2 (only the
 *      x >= n - 1 levels can trigger this);
 *   4. (2,3)-sparsity: the parent is already sparse, so only the subsets
 *      containing the new vertex are checked (2^(x-1) subsets).
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit, with
 * children of the same parent deduplicated by canonical form. The
 * canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic Laman graphs on n vertices =
 * OEIS A227117(n): 1, 1, 1, 1, 3, 13, 70, 608, 7222, 110132, ...
 * (Laman graphs are connected — c >= 2 components give
 * m <= 2n - 3c < 2n - 3 — so there is no `connected_only` flag.)
 *
 * References:
 *   Laman, "On graphs and rigidity of plane skeletal structures,"
 *   J. Engrg. Math. 4, 1970 (the characterization);
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path);
 *   Larsson, nauty-laman-plugin (github.com/martinkjlarsson/nauty-laman-plugin)
 *   (generation through (2,3)-sparse graphs);
 *   OEIS A227117
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic Laman enumeration
 */
enum class LamanUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path through (2,3)-sparse graphs */
};

/**
 * @brief An enumerated Laman graph
 */
struct LamanUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic Laman enumeration
 */
struct LamanUnlabeledEnumerationResult {
    std::vector<LamanUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/** @brief Population count (C++11-compatible, no compiler builtin) */
inline int laman_unlabeled_popcount(unsigned long long x) {
    int c = 0;
    while (x) {
        x &= x - 1;
        ++c;
    }
    return c;
}

/**
 * @brief (2,3)-sparsity of the subsets containing the newly added vertex
 * @param x Number of vertices of the extended graph (the new vertex is x - 1)
 * @param adj Adjacency bitmasks of the extended graph
 * @return true if every vertex subset containing x - 1 spans at most
 *         2k - 3 edges (k = subset size)
 *
 * The parent graph on vertices 0, ..., x - 2 is already (2,3)-sparse, so
 * only the 2^(x-1) subsets containing the new vertex need checking. Subsets
 * of size 2 can never violate the bound on a simple graph, and size-k
 * counting terminates early once the 2k - 3 limit is exceeded.
 */
inline bool laman_unlabeled_new_vertex_sparse(
    int x, const std::vector<unsigned long long>& adj) {
    if (x <= 2) return true;
    const int nv = x - 1;
    const unsigned long long all = (1ULL << nv) - 1;
    for (unsigned long long sub = 1; sub <= all; ++sub) {
        const unsigned long long s = sub | (1ULL << nv);
        const int limit = 2 * (laman_unlabeled_popcount(sub) + 1) - 3;
        int edges = 0;
        bool exceeded = false;
        for (int u = 0; u < x && !exceeded; ++u) {
            if (!((s >> u) & 1ULL)) continue;
            // Neighbors of u inside s with a larger index, so each edge
            // is counted exactly once.
            edges += laman_unlabeled_popcount(
                adj[u] & s & ~((1ULL << (u + 1)) - 1));
            if (edges > limit) exceeded = true;
        }
        if (exceeded) return false;
    }
    return true;
}

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline LamanUnlabeledEnumeratedGraph laman_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    LamanUnlabeledEnumeratedGraph g;
    g.n = k;
    g.edges = bitmask_graph_edges(k, adj);
    return g;
}

/**
 * @brief Canonical augmentation DFS from a k-vertex canonical representative
 * @param k Current number of vertices
 * @param adj Adjacency bitmasks of the current ((2,3)-sparse) graph
 * @param edge_count Number of edges of the current graph
 * @param n Target number of vertices
 * @param results Storage for results
 *
 * Extends by a new vertex whose neighborhood S runs over all subsets of the
 * current vertex set, keeping the children that stay (2,3)-sparse and can
 * still reach tightness (the four prunings in the file comment). A child
 * survives when the new vertex lies in its canonical-deletion orbit and its
 * canonical form was not already produced by a sibling; correctness of
 * accepting one parent per class is McKay's canonical construction path
 * argument. At the leaves the reachability pruning has forced
 * m >= 2n - 3 and sparsity forces m <= 2n - 3, so every leaf is tight;
 * the explicit check is kept for clarity.
 */
inline void laman_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int edge_count, int n,
    std::vector<LamanUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (edge_count == 2 * n - 3) {
            results.push_back(laman_unlabeled_build_graph(k, adj));
        }
        return;
    }
    const int x = k + 1;  // Vertex count after the extension
    // Maximum number of edges the vertices added after this one can
    // contribute: the j-th added vertex (0-indexed) has degree at most j.
    int max_future = 0;
    for (int j = x; j < n; ++j) max_future += j;

    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        const int new_edges = laman_unlabeled_popcount(s);
        const int m = edge_count + new_edges;

        // Pruning 1: sparsity edge bound on the full vertex set.
        if (x >= 2 && m > 2 * x - 3) continue;
        // Pruning 2: tightness reachability.
        if (m + max_future < 2 * n - 3) continue;

        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        bool viable = true;

        // Pruning 3: degree feasibility (can only fail when at most one
        // vertex is still to come, since deg(v) + (n - x) < 2 needs
        // n - x <= 1).
        if (n >= 3 && n - x <= 1) {
            for (int v = 0; v < x; ++v) {
                if (laman_unlabeled_popcount(adj[v]) + (n - x) < 2) {
                    viable = false;
                    break;
                }
            }
        }

        // Pruning 4: (2,3)-sparsity of the subsets containing the new vertex.
        if (viable && !laman_unlabeled_new_vertex_sparse(x, adj)) {
            viable = false;
        }

        if (viable) {
            CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(x, adj);
            if (((canon.last_orbit >> k) & 1ULL) &&
                child_forms.insert(canon.form).second) {
                laman_unlabeled_enum_dfs(x, adj, m, n, results);
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
 * @brief Enumerates all non-isomorphic Laman graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return LamanUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A227117(n) graphs
 * (1, 1, 1, 1, 3, 13, 70, 608, 7222, ... for n = 1, 2, ...). n = 1 yields
 * K_1 and n = 2 yields K_2, matching `check_laman` and the labeled
 * enumerator; n <= 0 and n >= 64 yield nothing (the empty graph is not
 * Laman). Laman graphs are always connected, so there is no
 * `connected_only` flag (same as the labeled enumerator).
 *
 * @note Every candidate child costs one incremental (2,3)-sparsity check
 *       (2^(x-1) subsets) plus, when it survives, one exact
 *       canonicalization (branch and bound over vertex orderings), so the
 *       enumeration is practical to about n = 9.
 */
inline LamanUnlabeledEnumerationResult
enumerate_laman_unlabeled_graphs(
    int n,
    LamanUnlabeledEnumAlgorithm algo =
        LamanUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    LamanUnlabeledEnumerationResult result;
    if (n <= 0 || n >= 64) return result;
    if (n == 1) {
        LamanUnlabeledEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::laman_unlabeled_enum_dfs(1, adj, 0, n, result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
