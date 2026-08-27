#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_UNLABELED_ENUM_H

/**
 * @file trivially_perfect_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic trivially perfect graphs
 *
 * Enumerates one representative per isomorphism class through the classical
 * bijection with rooted forests (Wolk): a rooted tree yields a connected
 * trivially perfect graph by connecting every vertex to all of its ancestors
 * (the comparability graph of the tree order), and disjoint unions of such
 * closures cover the disconnected graphs. The map is a bijection on
 * isomorphism classes, so no isomorph rejection is needed: in the closure the
 * universal vertices are exactly the chain from the root down to the first
 * vertex with zero or two-or-more children, and peeling that clique and
 * recursing on the components of the remainder (the unique universal vertex
 * decomposition) rebuilds the tree.
 *
 * Rooted trees are generated as canonical level sequences by the machinery of
 * tree_unlabeled_enum.h (Beyer--Hedetniemi); disconnected graphs are composed
 * from connected components over integer partitions, as
 * forest_unlabeled_enum.h does for trees.
 *
 * Number of non-isomorphic trivially perfect graphs on n vertices
 * = number of rooted forests on n nodes = OEIS A000081(n+1):
 *   1, 2, 4, 9, 20, 48, 115, 286, ...
 * Connected only = number of rooted trees on n nodes = A000081(n):
 *   1, 1, 2, 4, 9, 20, 48, 115, ...
 *
 * References:
 *   Wolk, "The comparability graph of a tree,"
 *   Proc. Amer. Math. Soc. 13(5), 1962
 *
 *   Beyer, Hedetniemi, "Constant Time Generation of Rooted Trees,"
 *   SIAM J. Comput. 9(4), 1980
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include "enumerators/tree_unlabeled_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic trivially perfect enumeration
 */
enum class TriviallyPerfectUnlabeledEnumAlgorithm {
    ROOTED_FOREST /**< Ancestor closure of canonically generated rooted forests */
};

/**
 * @brief An enumerated trivially perfect graph
 */
struct TriviallyPerfectUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic trivially perfect enumeration
 */
struct TriviallyPerfectUnlabeledEnumerationResult {
    std::vector<TriviallyPerfectUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Builds the ancestor closure of a rooted tree given as a level sequence
 *
 * @p L is a canonical level sequence (depths in DFS preorder, L[0] = 0) as
 * produced by compute_rooted_trees. Vertex i+1 is joined to every vertex on
 * the root path above it; preorder numbering makes every ancestor smaller
 * than its descendants, so edges come out with first < second directly.
 */
inline TriviallyPerfectUnlabeledEnumeratedGraph
trivially_perfect_unlabeled_closure_graph(const std::vector<int>& L) {
    int n = (int)L.size();
    TriviallyPerfectUnlabeledEnumeratedGraph g;
    g.n = n;

    // path[d] = vertex currently on the root path at depth d
    std::vector<int> path(n, 0);
    for (int i = 0; i < n; ++i) {
        int v = i + 1;
        int d = L[i];
        for (int a = 0; a < d; ++a) {
            g.edges.push_back(std::make_pair(path[a], v));
        }
        path[d] = v;
    }

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief Enumerates connected non-isomorphic trivially perfect graphs on n vertices
 *
 * One graph per canonical rooted tree of size n: the closure of a rooted tree
 * is connected (the root is universal), and non-isomorphic rooted trees have
 * non-isomorphic closures because the tree is recoverable from the closure
 * through the universal vertex decomposition.
 *
 * @param n Number of vertices
 * @param rooted_tree_cache Memoization cache shared across sizes
 */
inline std::vector<TriviallyPerfectUnlabeledEnumeratedGraph>
trivially_perfect_unlabeled_connected(
    int n, std::map<int, std::vector<std::vector<int> > >& rooted_tree_cache) {

    std::vector<TriviallyPerfectUnlabeledEnumeratedGraph> out;
    if (n < 1) return out;

    compute_rooted_trees(n, rooted_tree_cache);
    const std::vector<std::vector<int> >& trees = rooted_tree_cache[n];
    for (std::size_t t = 0; t < trees.size(); ++t) {
        out.push_back(trivially_perfect_unlabeled_closure_graph(trees[t]));
    }
    return out;
}

/**
 * @brief Recursive function that selects a component for each part and builds a graph
 *
 * Selects a connected graph for each part size in parts[idx..], applying
 * non-decreasing index constraints for equal-size parts.
 *
 * @param parts Partition (in non-increasing order)
 * @param idx Index of the currently processed part
 * @param prev_index Index of the component chosen for the previous part of equal size
 * @param vertex_offset Current vertex offset
 * @param current_edges Edge list under construction
 * @param connected_by_size List of connected graphs by size
 * @param results Storage for results
 * @param n Total number of vertices
 */
inline void trivially_perfect_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index, int vertex_offset,
    std::vector<std::pair<int, int> >& current_edges,
    const std::map<int, std::vector<TriviallyPerfectUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<TriviallyPerfectUnlabeledEnumeratedGraph>& results,
    int n) {

    if (idx == parts.size()) {
        TriviallyPerfectUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    int sz = parts[idx];
    const std::vector<TriviallyPerfectUnlabeledEnumeratedGraph>& comps =
        connected_by_size.find(sz)->second;

    // If a previous part of equal size exists, restrict index >= prev_index
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)comps.size(); ++i) {
        std::size_t old_size = current_edges.size();
        const std::vector<std::pair<int, int> >& comp_edges = comps[i].edges;
        for (std::size_t e = 0; e < comp_edges.size(); ++e) {
            int u = comp_edges[e].first + vertex_offset;
            int v = comp_edges[e].second + vertex_offset;
            current_edges.push_back(std::make_pair(u, v));
        }

        trivially_perfect_unlabeled_combine(parts, idx + 1, i, vertex_offset + sz,
                                            current_edges, connected_by_size, results, n);

        current_edges.resize(old_size);
    }
}

/**
 * @brief Enumerates integer partitions and builds a graph for each partition
 *
 * Enumerates all partitions of n as a sum of positive integers in non-increasing order.
 */
inline void trivially_perfect_unlabeled_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    const std::map<int, std::vector<TriviallyPerfectUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<TriviallyPerfectUnlabeledEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        trivially_perfect_unlabeled_combine(parts, 0, 0, 0, current_edges,
                                            connected_by_size, results, n);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        trivially_perfect_unlabeled_partition_dfs(remaining - s, s, parts,
                                                  connected_by_size, results, n);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic trivially perfect graphs on n vertices
 * @param n Number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: ROOTED_FOREST)
 * @return TriviallyPerfectUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000081(n+1) graphs
 * (1, 2, 4, 9, 20, 48, 115, 286, ... for n = 1, 2, ...), or A000081(n) graphs
 * (1, 1, 2, 4, 9, 20, 48, 115, ...) when @p connected_only is set.
 * n = 0 yields the single empty graph in both modes; negative n yields nothing.
 *
 * @note The result materializes every graph; the counts grow exponentially
 *       (A000081 passes two million around n = 16).
 */
inline TriviallyPerfectUnlabeledEnumerationResult
enumerate_trivially_perfect_unlabeled_graphs(
    int n, bool connected_only = false,
    TriviallyPerfectUnlabeledEnumAlgorithm algo =
        TriviallyPerfectUnlabeledEnumAlgorithm::ROOTED_FOREST) {
    (void)algo;
    TriviallyPerfectUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_trivially_perfect accepts n = 0; emit the empty graph.
        TriviallyPerfectUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::map<int, std::vector<std::vector<int> > > rooted_tree_cache;

    if (connected_only) {
        result.graphs = detail::trivially_perfect_unlabeled_connected(n, rooted_tree_cache);
        return result;
    }

    // Pre-compute connected graphs for each size
    std::map<int, std::vector<TriviallyPerfectUnlabeledEnumeratedGraph> > connected_by_size;
    for (int k = 1; k <= n; ++k) {
        connected_by_size[k] = detail::trivially_perfect_unlabeled_connected(k, rooted_tree_cache);
    }

    // Enumerate integer partitions and build graphs
    std::vector<int> parts;
    detail::trivially_perfect_unlabeled_partition_dfs(n, n, parts, connected_by_size,
                                                      result.graphs, n);

    return result;
}

}  // namespace graph_recognition

#endif
