#ifndef GRAPH_RECOGNITION_FOREST_ENUM_H
#define GRAPH_RECOGNITION_FOREST_ENUM_H

/**
 * @file forest_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic forests
 *
 * Enumerates all non-isomorphic forests on n vertices by composing tree enumerations.
 * Enumerates component size configurations via integer partitions, obtains
 * non-isomorphic trees of each size from tree_unlabeled_enum.h, and combines them.
 * For equal-size components, duplicates are eliminated by non-decreasing index constraints.
 *
 * Number of non-isomorphic forests: OEIS A005195
 *   1, 1, 2, 3, 6, 10, 20, 37, 76, 153, ...
 *
 * References:
 *   Harary, Palmer, "Graphical Enumeration,"
 *   Academic Press, 1973
 */

#include "enumerators/tree_unlabeled_enum.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for forest enumeration
 */
enum class ForestUnlabeledEnumAlgorithm {
    PARTITION_COMPOSE /**< Integer partition + tree composition */
};

/**
 * @brief An enumerated forest graph
 */
struct ForestUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of forest enumeration
 */
struct ForestUnlabeledEnumerationResult {
    std::vector<ForestUnlabeledEnumeratedGraph> graphs;    /**< Array of enumerated forests */
};

namespace detail {

/**
 * @brief Recursive function that selects trees for each component and builds a forest
 *
 * Selects a tree for each part size in parts[idx..], applying
 * non-decreasing index constraints for equal-size parts.
 *
 * @param parts Partition (in non-increasing order)
 * @param idx Index of the currently processed part
 * @param prev_index Index of the tree chosen for the previous part of equal size
 * @param vertex_offset Current vertex offset
 * @param current_edges Edge list under construction
 * @param trees_by_size List of trees by size
 * @param results Storage for results
 * @param n Total number of vertices
 */
inline void forest_combine(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index, int vertex_offset,
    std::vector<std::pair<int, int> >& current_edges,
    const std::map<int, std::vector<TreeUnlabeledEnumeratedGraph> >& trees_by_size,
    std::vector<ForestUnlabeledEnumeratedGraph>& results,
    int n) {

    if (idx == parts.size()) {
        ForestUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    int sz = parts[idx];
    const std::vector<TreeUnlabeledEnumeratedGraph>& trees = trees_by_size.find(sz)->second;

    // If a previous part of equal size exists, restrict index >= prev_index
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)trees.size(); ++i) {
        // Add tree edges with offset
        std::size_t old_size = current_edges.size();
        const std::vector<std::pair<int, int> >& tree_edges = trees[i].edges;
        for (std::size_t e = 0; e < tree_edges.size(); ++e) {
            int u = tree_edges[e].first + vertex_offset;
            int v = tree_edges[e].second + vertex_offset;
            current_edges.push_back(std::make_pair(u, v));
        }

        forest_combine(parts, idx + 1, i, vertex_offset + sz,
                        current_edges, trees_by_size, results, n);

        current_edges.resize(old_size);
    }
}

/**
 * @brief Enumerates integer partitions and builds a forest for each partition
 *
 * Enumerates all partitions of n as a sum of positive integers in non-increasing order.
 */
inline void forest_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    const std::map<int, std::vector<TreeUnlabeledEnumeratedGraph> >& trees_by_size,
    std::vector<ForestUnlabeledEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        forest_combine(parts, 0, 0, 0, current_edges, trees_by_size, results, n);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        forest_partition_dfs(remaining - s, s, parts, trees_by_size, results, n);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic forests on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: PARTITION_COMPOSE)
 * @return ForestUnlabeledEnumerationResult
 *
 * Enumerates component size configurations via integer partitions and
 * combines non-isomorphic trees of each size to generate all non-isomorphic forests.
 */
inline ForestUnlabeledEnumerationResult enumerate_forest_unlabeled_graphs(int n,
    ForestUnlabeledEnumAlgorithm algo = ForestUnlabeledEnumAlgorithm::PARTITION_COMPOSE) {
    (void)algo;
    ForestUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_forest accepts n = 0; emit the empty forest.
        ForestUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    // Pre-compute non-isomorphic trees for each size
    std::map<int, std::vector<TreeUnlabeledEnumeratedGraph> > trees_by_size;
    for (int k = 1; k <= n; ++k) {
        TreeUnlabeledEnumerationResult tres = enumerate_tree_unlabeled_graphs(k);
        trees_by_size[k] = tres.graphs;
    }

    // Enumerate integer partitions and build forests
    std::vector<int> parts;
    detail::forest_partition_dfs(n, n, parts, trees_by_size, result.graphs, n);

    return result;
}

}  // namespace graph_recognition

#endif
