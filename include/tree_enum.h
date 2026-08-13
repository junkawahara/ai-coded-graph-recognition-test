#ifndef GRAPH_RECOGNITION_TREE_ENUM_H
#define GRAPH_RECOGNITION_TREE_ENUM_H

/**
 * @file tree_enum.h
 * @brief Non-isomorphic free tree enumeration
 *
 * Enumerates all non-isomorphic free trees on n vertices via
 * bottom-up recursive construction + centroid decomposition.
 *
 * Rooted trees are represented as level sequences (depth sequences in DFS preorder),
 * and all canonical rooted trees are generated via integer partition + memoized recursion.
 * Free trees are uniquely represented by rooting at the centroid:
 *   - Single-centroid tree: all root subtree sizes <= floor((n-1)/2)
 *   - Bi-centroid tree (n even): rooted tree pair (T_i, T_j) of size n/2, i <= j
 *
 * Non-isomorphic count: OEIS A000055
 *   1, 1, 1, 2, 3, 6, 11, 23, 47, 106, ...
 *
 * References:
 *   Wright, Richmond, Odlyzko, McKay,
 *   "Constant Time Generation of Free Trees,"
 *   SIAM J. Comput. 15(2), 1986
 *
 *   Beyer, Hedetniemi,
 *   "Constant Time Generation of Rooted Trees,"
 *   SIAM J. Comput. 9(4), 1980
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for tree enumeration
 */
enum class TreeEnumAlgorithm {
    LEVEL_SEQUENCE /**< Constructive enumeration via level sequence */
};

/**
 * @brief Enumerated tree graph
 */
struct TreeEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted by u < v) */
};

/**
 * @brief Result of tree enumeration
 */
struct TreeEnumerationResult {
    std::vector<TreeEnumeratedGraph> graphs;      /**< Array of enumerated trees */
};

namespace detail {

/**
 * @brief Build edge list from level sequence (single rooted tree)
 *
 * Generates a 1-indexed vertex edge list from level sequence L (0-indexed entries, L[0]=0).
 * The parent of vertex i+1 is the most recent vertex at depth L[i]-1.
 */
inline TreeEnumeratedGraph level_seq_to_tree_graph(
    const std::vector<int>& L, int n) {

    TreeEnumeratedGraph g;
    g.n = n;
    if (n <= 1) return g;

    // depth_last[d] = last vertex at depth d (1-indexed)
    std::vector<int> depth_last(n, 0);
    depth_last[0] = 1;

    for (int i = 1; i < n; ++i) {
        int vertex = i + 1;
        int parent = depth_last[L[i] - 1];
        int u = (parent < vertex) ? parent : vertex;
        int v = (parent < vertex) ? vertex : parent;
        g.edges.push_back(std::make_pair(u, v));
        depth_last[L[i]] = vertex;
    }

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief Build bi-centroid tree by joining two rooted trees at their roots
 *
 * T1 vertices: 1..half, T2 vertices: half+1..2*half
 * Bridge edge: (1, half+1)
 */
inline TreeEnumeratedGraph join_rooted_trees(
    const std::vector<int>& L1, const std::vector<int>& L2) {

    int half = (int)L1.size();
    TreeEnumeratedGraph g;
    g.n = 2 * half;

    // T1 edges (vertices 1..half)
    {
        std::vector<int> depth_last(half, 0);
        depth_last[0] = 1;
        for (int i = 1; i < half; ++i) {
            int vertex = i + 1;
            int parent = depth_last[L1[i] - 1];
            int u = (parent < vertex) ? parent : vertex;
            int v = (parent < vertex) ? vertex : parent;
            g.edges.push_back(std::make_pair(u, v));
            depth_last[L1[i]] = vertex;
        }
    }

    // T2 edges (vertices half+1..2*half)
    {
        std::vector<int> depth_last(half, 0);
        depth_last[0] = half + 1;
        for (int i = 1; i < half; ++i) {
            int vertex = half + i + 1;
            int parent = depth_last[L2[i] - 1];
            int u = (parent < vertex) ? parent : vertex;
            int v = (parent < vertex) ? vertex : parent;
            g.edges.push_back(std::make_pair(u, v));
            depth_last[L2[i]] = vertex;
        }
    }

    // Bridge edge
    g.edges.push_back(std::make_pair(1, half + 1));

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief Compute subtree sizes of the root's direct children
 */
inline std::vector<int> root_subtree_sizes(const std::vector<int>& L, int n) {
    std::vector<int> sizes;
    if (n <= 1) return sizes;

    int i = 1;
    while (i < n) {
        // L[i] == 1, so a new subtree begins here
        int start = i;
        ++i;
        while (i < n && L[i] > 1) {
            ++i;
        }
        sizes.push_back(i - start);
    }
    return sizes;
}

/**
 * @brief Assemble level sequence (root + subtree sequences)
 *
 * Shifts subtree level sequences by +1 and concatenates them.
 */
inline std::vector<int> assemble_level_sequence(
    const std::vector<std::vector<int> >& subtrees) {

    int total = 1;
    for (std::size_t i = 0; i < subtrees.size(); ++i) {
        total += (int)subtrees[i].size();
    }

    std::vector<int> L;
    L.reserve(total);
    L.push_back(0);  // root

    for (std::size_t i = 0; i < subtrees.size(); ++i) {
        const std::vector<int>& sub = subtrees[i];
        for (std::size_t j = 0; j < sub.size(); ++j) {
            L.push_back(sub[j] + 1);
        }
    }

    return L;
}

/**
 * @brief Recursively generate combinations of subtrees
 *
 * Selects rooted trees for each part size in parts[idx..], and
 * Applies non-decreasing index constraint for equal-size parts.
 */
inline void tree_enum_combine_subtrees(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index,
    std::vector<std::vector<int> >& chosen,
    std::map<int, std::vector<std::vector<int> > >& cache,
    std::vector<std::vector<int> >& results) {

    if (idx == parts.size()) {
        results.push_back(assemble_level_sequence(chosen));
        return;
    }

    int sz = parts[idx];
    const std::vector<std::vector<int> >& trees = cache[sz];

    // If there is a preceding part of equal size, restrict index >= prev_index
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)trees.size(); ++i) {
        chosen.push_back(trees[i]);
        tree_enum_combine_subtrees(parts, idx + 1, i, chosen, cache, results);
        chosen.pop_back();
    }
}

/**
 * @brief Enumerate integer partitions and generate subtree combinations for each
 */
inline void tree_enum_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    std::map<int, std::vector<std::vector<int> > >& cache,
    std::vector<std::vector<int> >& results) {

    if (remaining == 0) {
        std::vector<std::vector<int> > chosen;
        tree_enum_combine_subtrees(parts, 0, 0, chosen, cache, results);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        tree_enum_partition_dfs(remaining - s, s, parts, cache, results);
        parts.pop_back();
    }
}

/**
 * @brief Generate all canonical rooted trees of size k as level sequences (memoized)
 */
inline void compute_rooted_trees(
    int k,
    std::map<int, std::vector<std::vector<int> > >& cache) {

    if (cache.count(k)) return;

    if (k == 1) {
        std::vector<int> single(1, 0);
        cache[1].push_back(single);
        return;
    }

    // Compute smaller sizes first
    for (int i = 1; i < k; ++i) {
        compute_rooted_trees(i, cache);
    }

    // Enumerate integer partitions of (k-1) and combine subtrees for each
    std::vector<int> parts;
    tree_enum_partition_dfs(k - 1, k - 1, parts, cache, cache[k]);

    // Lexicographic sort
    std::sort(cache[k].begin(), cache[k].end());
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic free trees on n vertices
 * @param n Number of vertices
 * @param algo Algorithm to use (default: LEVEL_SEQUENCE)
 * @return TreeEnumerationResult
 *
 * Using memoized recursive rooted tree generation + centroid decomposition,
 * outputs exactly one representative per isomorphism class.
 * For n = 0 the single empty tree is returned; for n < 0 the result is empty.
 */
inline TreeEnumerationResult enumerate_tree_graphs(int n,
    TreeEnumAlgorithm algo = TreeEnumAlgorithm::LEVEL_SEQUENCE) {
    (void)algo;
    TreeEnumerationResult result;
    if (n < 0) return result;

    if (n == 0) {
        // The empty graph is a tree (check_tree(n=0) is YES)
        TreeEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 1) {
        TreeEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 2) {
        TreeEnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }

    // Generate rooted trees of all sizes
    std::map<int, std::vector<std::vector<int> > > cache;
    for (int k = 1; k <= n; ++k) {
        detail::compute_rooted_trees(k, cache);
    }

    // --- Single-centroid trees ---
    // Extract from cache[n] those where all root subtree sizes are <= floor((n-1)/2)
    int threshold = (n - 1) / 2;
    const std::vector<std::vector<int> >& rooted_n = cache[n];
    for (std::size_t t = 0; t < rooted_n.size(); ++t) {
        std::vector<int> sizes = detail::root_subtree_sizes(rooted_n[t], n);
        int max_sz = 0;
        for (std::size_t i = 0; i < sizes.size(); ++i) {
            if (sizes[i] > max_sz) max_sz = sizes[i];
        }
        if (max_sz <= threshold) {
            result.graphs.push_back(
                detail::level_seq_to_tree_graph(rooted_n[t], n));
        }
    }

    // --- Bi-centroid trees (even n only) ---
    if (n % 2 == 0) {
        int half = n / 2;
        const std::vector<std::vector<int> >& rooted_half = cache[half];
        int cnt = (int)rooted_half.size();
        for (int i = 0; i < cnt; ++i) {
            for (int j = i; j < cnt; ++j) {
                result.graphs.push_back(
                    detail::join_rooted_trees(rooted_half[i], rooted_half[j]));
            }
        }
    }

    return result;
}

}  // namespace graph_recognition

#endif
