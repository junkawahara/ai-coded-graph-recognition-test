#ifndef GRAPH_RECOGNITION_LAMAN_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_LAMAN_UNLABELED_ENUM_H

/**
 * @file laman_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic Laman graphs
 *
 * Enumerates one representative per isomorphism class of Laman graphs
 * (minimally rigid graphs in 2D, (2,3)-tight graphs) on n vertices.
 *
 * HENNEBERG (default) starts from K2 and applies both extensions:
 *   I. add a vertex adjacent to any two distinct existing vertices;
 *  II. delete an edge uv, add a vertex adjacent to u, v and a third
 *      existing vertex w distinct from u and v.
 * Both operations preserve Laman tightness, and every Laman graph with
 * n >= 2 has such a construction. All intermediates are therefore Laman,
 * without recognition or subset-sparsity checks. At each size, a shared
 * canonical-form set removes duplicates from all parents and operations.
 * A generic last-vertex deletion rule would be invalid here: an inverse
 * Henneberg II move also restores an edge and must preserve sparsity.
 * Only the current and next levels are retained, not construction histories.
 *
 * CANONICAL_AUGMENTATION retains McKay's canonical construction path
 * method through sparse intermediates, the same route as the
 * nauty-laman-plugin (geng growing through (2,3)-sparse graphs). Laman
 * graphs themselves are not closed under vertex deletion (the edge count
 * 2n - 3 breaks), but (2,3)-sparsity is hereditary, so the intermediate
 * levels range over (2,3)-sparse graphs and tightness (m = 2n - 3) filters
 * at emission. Every Laman graph is reachable because deleting the
 * canonically last vertex of a sparse graph again yields a sparse graph.
 *
 * Legacy pruning on every candidate child (a new vertex with neighborhood S),
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
 * Legacy isomorph rejection uses the canonical-parent test: canonicalizing
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
 *   Taufiqurrachman, Yamauchi, Kijima, Yamashita, "Enumeration of Henneberg
 *   decompositions of Laman graphs and M-Circuits," Sections 2.4 and 3.1,
 *   https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf
 *   (forward construction with isomorph rejection);
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
#include <stdexcept>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic Laman enumeration
 */
enum class LamanUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION = 0, /**< Previous canonical search through (2,3)-sparse graphs */
    HENNEBERG = 1 /**< Henneberg I/II extensions with per-level isomorph rejection (default) */
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

// All operations use zero-based adjacency bitmasks. The public API guards
// n <= 63, matching the shared canonicalizer's supported range.
typedef std::vector<unsigned long long> LamanUnlabeledBitmaskGraph;

// Generate every Henneberg extension of a Laman parent on k >= 2 vertices.
// The callback borrows the child buffer and must copy it to retain it.
template <typename Callback>
inline void laman_unlabeled_henneberg_children(
    const LamanUnlabeledBitmaskGraph& parent, Callback emit) {
    const int k = static_cast<int>(parent.size());
    const unsigned long long new_bit = 1ULL << k;
    LamanUnlabeledBitmaskGraph adj = parent;
    adj.push_back(0ULL);
    for (int u = 0; u < k; ++u) {
        for (int v = u + 1; v < k; ++v) {
            // Henneberg I: u and v need not be adjacent.
            adj[k] = (1ULL << u) | (1ULL << v);
            adj[u] |= new_bit;
            adj[v] |= new_bit;
            emit(adj);

            if ((parent[u] >> v) & 1ULL) {
                // Henneberg II: replace uv with u--new--v and connect
                // new to each possible third vertex. Restore uv afterwards.
                adj[u] &= ~(1ULL << v);
                adj[v] &= ~(1ULL << u);
                for (int w = 0; w < k; ++w) {
                    if (w == u || w == v) continue;
                    adj[k] |= 1ULL << w;
                    adj[w] |= new_bit;
                    emit(adj);
                    adj[w] &= ~new_bit;
                    adj[k] &= ~(1ULL << w);
                }
                adj[u] |= 1ULL << v;
                adj[v] |= 1ULL << u;
            }
            adj[u] &= ~new_bit;
            adj[v] &= ~new_bit;
        }
    }
}

// Return all non-isomorphic representatives on n >= 2 vertices. Applying
// every extension to one representative of each parent class is sufficient:
// an isomorphism transports every choice of endpoints to that representative.
inline std::vector<LamanUnlabeledBitmaskGraph>
laman_unlabeled_henneberg_graphs(int n) {
    LamanUnlabeledBitmaskGraph k2;
    k2.push_back(2ULL);
    k2.push_back(1ULL);
    std::vector<LamanUnlabeledBitmaskGraph> current(1, k2);
    for (int k = 2; k < n; ++k) {
        std::vector<LamanUnlabeledBitmaskGraph> next;
        std::set<std::vector<unsigned long long> > seen;
        for (std::size_t i = 0; i < current.size(); ++i) {
            laman_unlabeled_henneberg_children(current[i],
                [&](const LamanUnlabeledBitmaskGraph& child) {
                    const CanonicalAugmentationCanon canon =
                        canonicalize_bitmask_graph(k + 1, child);
                    if (seen.insert(canon.form).second) next.push_back(child);
                });
        }
        current.swap(next);
    }
    return current;
}

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
 * @param n Number of vertices (0 <= n <= 63)
 * @param algo Algorithm to use (default: HENNEBERG)
 * @return LamanUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A227117(n) graphs
 * (1, 1, 1, 1, 3, 13, 70, 608, 7222, ... for n = 1, 2, ...). n = 1 yields
 * K_1 and n = 2 yields K_2, matching `check_laman` and the labeled
 * enumerator; n = 0 yields nothing (the empty graph is not
 * Laman). Laman graphs are always connected, so there is no
 * `connected_only` flag (same as the labeled enumerator).
 *
 * HENNEBERG tries binom(k,2) + (2k-3)(k-2) extensions per k-vertex parent.
 * Each needs the shared exact, worst-case factorial canonicalizer. It
 * retains the current/next levels and the next level's canonical-form set.
 * No polynomial-delay guarantee is claimed. CANONICAL_AUGMENTATION keeps
 * the previous subset checks, canonical-parent search and output order.
 * Both algorithms materialize all outputs, but may use different orders
 * and representative labelings.
 * @throws std::invalid_argument if n < 0, n > 63 (adjacency bitmask limit),
 *         or algo is unknown
 */
inline LamanUnlabeledEnumerationResult
enumerate_laman_unlabeled_graphs(
    int n,
    LamanUnlabeledEnumAlgorithm algo =
        LamanUnlabeledEnumAlgorithm::HENNEBERG) {
    LamanUnlabeledEnumerationResult result;
    require_vertex_count("enumerate_laman_unlabeled_graphs", n, 63);
    if (algo != LamanUnlabeledEnumAlgorithm::HENNEBERG &&
        algo != LamanUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
        throw std::invalid_argument("enumerate_laman_unlabeled_graphs: unknown algorithm");
    }
    if (n <= 0) return result;
    if (n == 1) {
        LamanUnlabeledEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }
    if (algo == LamanUnlabeledEnumAlgorithm::HENNEBERG) {
        const std::vector<detail::LamanUnlabeledBitmaskGraph> representatives =
            detail::laman_unlabeled_henneberg_graphs(n);
        for (std::size_t i = 0; i < representatives.size(); ++i) {
            result.graphs.push_back(detail::laman_unlabeled_build_graph(n, representatives[i]));
        }
    } else {
        std::vector<unsigned long long> adj(1, 0);
        detail::laman_unlabeled_enum_dfs(1, adj, 0, n, result.graphs);
    }
    return result;
}

}  // namespace graph_recognition

#endif
