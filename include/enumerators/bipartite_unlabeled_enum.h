#ifndef GRAPH_RECOGNITION_BIPARTITE_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_UNLABELED_ENUM_H

/**
 * @file bipartite_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic bipartite graphs
 *
 * Enumerates one representative per isomorphism class of bipartite graphs on
 * n vertices by McKay's canonical construction path method, the isomorph
 * rejection scheme behind nauty's geng / genbg. Graphs are grown one vertex
 * at a time, which is sound because bipartiteness is hereditary.
 *
 * The bipartiteness pruning is a 2-coloring test, not a recognizer call:
 * a connected bipartite graph has exactly one 2-coloring up to swapping the
 * two sides, so adding a vertex x with neighborhood S keeps the graph
 * bipartite if and only if, for every connected component C of the current
 * graph, S n C lies entirely inside one side of C's bipartition (flip that
 * component's coloring so this side is opposite to x, independently per
 * component; every cycle through x then closes at even length).
 *
 * Isomorph rejection is the canonical-parent test: one canonicalization of
 * each candidate child yields both its canonical form and the automorphism
 * orbit of the vertex placed last by the canonical labeling, and the child
 * survives only when the newly added vertex lies in that orbit (so each
 * class accepts exactly one parent class), with children of the same parent
 * deduplicated by canonical form (so that parent produces the class once).
 * The canonicalization itself lives in `util/canonical_augmentation.h`.
 *
 * Number of non-isomorphic bipartite graphs on n vertices =
 * OEIS A033995(n): 1, 2, 3, 7, 13, 35, 88, 303, 1119, 5479, ...
 * The connected ones are counted by A005142: 1, 1, 1, 3, 5, 17, 44, 182, ...
 *
 * References:
 *   McKay, "Isomorph-free exhaustive generation," J. Algorithms 26, 1998
 *   (canonical construction path; geng / genbg);
 *   Gainer-Dewar, Gessel, "Enumeration of bipartite graphs and bipartite
 *   blocks," Electron. J. Combin. 21(2), 2014; OEIS A033995, A005142
 */

#include <cstddef>
#include <set>
#include <utility>
#include <vector>

#include "util/canonical_augmentation.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic bipartite enumeration
 */
enum class BipartiteUnlabeledEnumAlgorithm {
    CANONICAL_AUGMENTATION /**< McKay canonical construction path (genbg style) */
};

/**
 * @brief An enumerated bipartite graph
 */
struct BipartiteUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic bipartite enumeration
 */
struct BipartiteUnlabeledEnumerationResult {
    std::vector<BipartiteUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief The two sides of one connected component's bipartition
 *
 * `first` and `second` are disjoint vertex bitmasks whose union is the
 * component. Which one is which is arbitrary (a connected bipartite graph
 * has exactly two 2-colorings, swapped), and the caller relies only on the
 * partition, never on the naming.
 */
struct BipartiteUnlabeledComponentSides {
    unsigned long long first;   /**< One side of the component */
    unsigned long long second;  /**< The other side */
};

/**
 * @brief Splits each connected component into its two colour classes
 * @param k Number of vertices
 * @param adj Adjacency bitmasks of a bipartite graph on 0, ..., k-1
 * @return One entry per connected component
 *
 * BFS 2-coloring. The graph is bipartite by the search invariant, so no
 * conflict can arise and none is checked for.
 */
inline std::vector<BipartiteUnlabeledComponentSides>
bipartite_unlabeled_component_sides(int k,
                                    const std::vector<unsigned long long>& adj) {
    std::vector<BipartiteUnlabeledComponentSides> sides;
    unsigned long long visited = 0;
    std::vector<int> queue;
    for (int s = 0; s < k; ++s) {
        if ((visited >> s) & 1ULL) continue;
        BipartiteUnlabeledComponentSides comp;
        comp.first = 1ULL << s;
        comp.second = 0;
        visited |= 1ULL << s;
        queue.clear();
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            bool u_in_first = ((comp.first >> u) & 1ULL) != 0;
            unsigned long long fresh = adj[u] & ~visited;
            for (int v = 0; v < k; ++v) {
                if (!((fresh >> v) & 1ULL)) continue;
                visited |= 1ULL << v;
                if (u_in_first) {
                    comp.second |= 1ULL << v;
                } else {
                    comp.first |= 1ULL << v;
                }
                queue.push_back(v);
            }
        }
        sides.push_back(comp);
    }
    return sides;
}

/** @brief Converts the bitmask adjacency to an enumerated graph */
inline BipartiteUnlabeledEnumeratedGraph bipartite_unlabeled_build_graph(
    int k, const std::vector<unsigned long long>& adj) {
    BipartiteUnlabeledEnumeratedGraph g;
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
 * Extends by a new vertex whose neighborhood S runs over the subsets meeting
 * each component in only one side of its bipartition (exactly the subsets
 * that keep the graph bipartite). A child survives when the new vertex lies
 * in its canonical-deletion orbit and its canonical form was not already
 * produced by a sibling; correctness of accepting one parent per class is
 * McKay's canonical construction path argument. Connectivity cannot be
 * pruned during the search (later vertices may join components), so
 * `connected_only` filters at emission.
 */
inline void bipartite_unlabeled_enum_dfs(
    int k, std::vector<unsigned long long>& adj, int n, bool connected_only,
    std::vector<BipartiteUnlabeledEnumeratedGraph>& results) {
    if (k == n) {
        if (!connected_only || bitmask_graph_connected(k, adj)) {
            results.push_back(bipartite_unlabeled_build_graph(k, adj));
        }
        return;
    }
    const std::vector<BipartiteUnlabeledComponentSides> sides =
        bipartite_unlabeled_component_sides(k, adj);
    std::set<std::vector<unsigned long long> > child_forms;
    const unsigned long long limit = 1ULL << k;
    for (unsigned long long s = 0; s < limit; ++s) {
        bool keeps_bipartite = true;
        for (size_t c = 0; c < sides.size() && keeps_bipartite; ++c) {
            if ((s & sides[c].first) != 0 && (s & sides[c].second) != 0) {
                keeps_bipartite = false;
            }
        }
        if (!keeps_bipartite) continue;

        adj.push_back(s);
        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] |= 1ULL << k;
        }

        CanonicalAugmentationCanon canon = canonicalize_bitmask_graph(k + 1, adj);
        if (((canon.last_orbit >> k) & 1ULL) &&
            child_forms.insert(canon.form).second) {
            bipartite_unlabeled_enum_dfs(k + 1, adj, n, connected_only, results);
        }

        for (int u = 0; u < k; ++u) {
            if ((s >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        }
        adj.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic bipartite graphs on n vertices
 * @param n Number of vertices (must be < 64; see the note on practical range)
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: CANONICAL_AUGMENTATION)
 * @return BipartiteUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A033995(n) graphs
 * (1, 2, 3, 7, 13, 35, 88, 303, ... for n = 1, 2, ...), or A005142(n)
 * (1, 1, 1, 3, 5, 17, 44, 182, ...) with @p connected_only. n = 0 yields
 * the single empty graph in both modes; negative n and n >= 64 yield
 * nothing.
 *
 * @note The canonicalization is exact branch-and-bound over vertex
 *       orderings (worst case k! on vertex-transitive graphs such as the
 *       empty graph), so the enumeration is practical to about n = 10
 *       (n = 9 takes about 1.5 seconds, n = 10 about half a minute;
 *       A033995(10) = 5479).
 */
inline BipartiteUnlabeledEnumerationResult enumerate_bipartite_unlabeled_graphs(
    int n, bool connected_only = false,
    BipartiteUnlabeledEnumAlgorithm algo =
        BipartiteUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION) {
    (void)algo;
    BipartiteUnlabeledEnumerationResult result;
    if (n < 0 || n >= 64) return result;
    if (n == 0) {
        BipartiteUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }
    std::vector<unsigned long long> adj(1, 0);
    detail::bipartite_unlabeled_enum_dfs(1, adj, n, connected_only,
                                         result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
