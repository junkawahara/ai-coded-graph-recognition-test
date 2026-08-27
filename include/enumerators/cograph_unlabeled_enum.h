#ifndef GRAPH_RECOGNITION_COGRAPH_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_COGRAPH_UNLABELED_ENUM_H

/**
 * @file cograph_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic cographs
 *
 * Enumerates one representative per isomorphism class by recursive cotree
 * construction through the union/join complement duality: for n >= 2 the
 * complement swaps the root type of the cotree, so complementation is a
 * bijection between the connected cographs (join root) and the disconnected
 * cographs (union root) on n vertices. Disconnected cographs are composed
 * from smaller connected ones over integer partitions, and the connected
 * ones are their complements; recursion on the component sizes grounds out
 * at the single vertex.
 *
 * No isomorph rejection is needed: two disjoint unions are isomorphic iff
 * their multisets of connected components are (uniqueness of the component
 * decomposition), the non-decreasing index constraint on equal-size parts
 * emits each multiset once, and complementation is injective on isomorphism
 * classes. The linear-delay generator of Jones, Protti and Del-Vecchio walks
 * the same cotree recursion; this implementation materializes the output
 * instead of achieving their delay bound.
 *
 * Number of non-isomorphic cographs on n vertices = OEIS A000084(n):
 *   1, 2, 4, 10, 24, 66, 180, 522, 1532, ...
 * Connected only = OEIS A000669(n) (series-reduced planted trees with n
 * leaves = cotrees with join root):
 *   1, 1, 2, 5, 12, 33, 90, 261, 766, ...
 * A000084(n) = 2 * A000669(n) for n >= 2 by the complement bijection.
 *
 * References:
 *   Seinsche, "On a property of the class of n-colorable graphs,"
 *   J. Combin. Theory Ser. B 16(2), 1974
 *
 *   Jones, Protti, Del-Vecchio, "Enumerating all cographs,"
 *   Theoret. Comput. Sci. 713, 2018
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic cograph enumeration
 */
enum class CographUnlabeledEnumAlgorithm {
    COTREE /**< Recursive cotree construction via the complement duality */
};

/**
 * @brief An enumerated cograph
 */
struct CographUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic cograph enumeration
 */
struct CographUnlabeledEnumerationResult {
    std::vector<CographUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Complements every graph of a list on the same vertex set
 *
 * Edges of the complement are generated with u < v in lexicographic order,
 * so the output edge lists are sorted without an extra sort.
 */
inline std::vector<CographUnlabeledEnumeratedGraph>
cograph_unlabeled_complement_all(
    const std::vector<CographUnlabeledEnumeratedGraph>& graphs) {

    std::vector<CographUnlabeledEnumeratedGraph> out;
    out.reserve(graphs.size());
    for (std::size_t i = 0; i < graphs.size(); ++i) {
        int n = graphs[i].n;
        std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));
        const std::vector<std::pair<int, int> >& edges = graphs[i].edges;
        for (std::size_t e = 0; e < edges.size(); ++e) {
            adj[edges[e].first][edges[e].second] = true;
        }

        CographUnlabeledEnumeratedGraph g;
        g.n = n;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if (!adj[u][v]) g.edges.push_back(std::make_pair(u, v));
            }
        }
        out.push_back(g);
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
inline void cograph_unlabeled_combine(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index, int vertex_offset,
    std::vector<std::pair<int, int> >& current_edges,
    const std::map<int, std::vector<CographUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<CographUnlabeledEnumeratedGraph>& results,
    int n) {

    if (idx == parts.size()) {
        CographUnlabeledEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    int sz = parts[idx];
    const std::vector<CographUnlabeledEnumeratedGraph>& comps =
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

        cograph_unlabeled_combine(parts, idx + 1, i, vertex_offset + sz,
                                  current_edges, connected_by_size, results, n);

        current_edges.resize(old_size);
    }
}

/**
 * @brief Enumerates integer partitions and builds a graph for each partition
 *
 * Enumerates all partitions of @p remaining into parts of size at most
 * @p max_part in non-increasing order.
 */
inline void cograph_unlabeled_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    const std::map<int, std::vector<CographUnlabeledEnumeratedGraph> >& connected_by_size,
    std::vector<CographUnlabeledEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        cograph_unlabeled_combine(parts, 0, 0, 0, current_edges,
                                  connected_by_size, results, n);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        cograph_unlabeled_partition_dfs(remaining - s, s, parts,
                                        connected_by_size, results, n);
        parts.pop_back();
    }
}

/**
 * @brief Enumerates the disconnected non-isomorphic cographs on k vertices
 *
 * Capping the part size at k - 1 forces at least two parts, so exactly the
 * disconnected graphs come out. @p connected_by_size must already hold the
 * connected cographs of every size below k.
 */
inline std::vector<CographUnlabeledEnumeratedGraph>
cograph_unlabeled_disconnected(
    int k,
    const std::map<int, std::vector<CographUnlabeledEnumeratedGraph> >& connected_by_size) {

    std::vector<CographUnlabeledEnumeratedGraph> out;
    std::vector<int> parts;
    cograph_unlabeled_partition_dfs(k, k - 1, parts, connected_by_size, out, k);
    return out;
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic cographs on n vertices
 * @param n Number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: COTREE)
 * @return CographUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000084(n) graphs
 * (1, 2, 4, 10, 24, 66, 180, 522, ... for n = 1, 2, ...), or A000669(n)
 * graphs (1, 1, 2, 5, 12, 33, 90, 261, ...) when @p connected_only is set;
 * the connected graphs come first in the output. n = 0 yields the single
 * empty graph in both modes; negative n yields nothing.
 *
 * @note The result materializes every graph; the counts grow exponentially
 *       (A000084 passes one million around n = 17).
 */
inline CographUnlabeledEnumerationResult enumerate_cograph_unlabeled_graphs(
    int n, bool connected_only = false,
    CographUnlabeledEnumAlgorithm algo = CographUnlabeledEnumAlgorithm::COTREE) {
    (void)algo;
    CographUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_cograph accepts n = 0; emit the empty graph.
        CographUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    // Build the connected cographs bottom-up: those on k vertices are the
    // complements of the disconnected ones, which are composed of connected
    // graphs on fewer than k vertices.
    std::map<int, std::vector<CographUnlabeledEnumeratedGraph> > connected_by_size;
    CographUnlabeledEnumeratedGraph single;
    single.n = 1;
    connected_by_size[1] = std::vector<CographUnlabeledEnumeratedGraph>(1, single);

    std::vector<CographUnlabeledEnumeratedGraph> disconnected_n;
    for (int k = 2; k <= n; ++k) {
        std::vector<CographUnlabeledEnumeratedGraph> disc =
            detail::cograph_unlabeled_disconnected(k, connected_by_size);
        connected_by_size[k] = detail::cograph_unlabeled_complement_all(disc);
        if (k == n) disconnected_n.swap(disc);
    }

    result.graphs = connected_by_size[n];
    if (!connected_only) {
        result.graphs.insert(result.graphs.end(),
                             disconnected_n.begin(), disconnected_n.end());
    }
    return result;
}

}  // namespace graph_recognition

#endif
