#ifndef GRAPH_RECOGNITION_CLUSTER_UNLABELED_ENUM_H
#define GRAPH_RECOGNITION_CLUSTER_UNLABELED_ENUM_H

/**
 * @file cluster_unlabeled_enum.h
 * @brief Enumeration of non-isomorphic cluster graphs
 *
 * A cluster graph is a disjoint union of cliques, so it is determined up to
 * isomorphism by the multiset of its clique sizes: the isomorphism classes on
 * n vertices are in bijection with the integer partitions of n. Enumerating
 * the partitions of n in non-increasing order and laying out one clique per
 * part on consecutive vertex blocks therefore emits exactly one representative
 * per isomorphism class, with no isomorph rejection and no filtering.
 *
 * The bijection is immediate in both directions: the cliques of a cluster
 * graph are its connected components, so the size multiset is an isomorphism
 * invariant, and two cluster graphs with the same size multiset are isomorphic
 * by matching up components of equal size.
 *
 * Number of non-isomorphic cluster graphs on n vertices = number of integer
 * partitions of n = OEIS A000041(n):
 *   1, 2, 3, 5, 7, 11, 15, 22, 30, 42, ...
 * The connected ones are exactly the complete graphs, so `connected_only`
 * always emits the single graph K_n.
 *
 * References:
 *   Knuth, "The Art of Computer Programming" Vol. 4A, 7.2.1.4
 *   (generation of all partitions); OEIS A000041
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for non-isomorphic cluster enumeration
 */
enum class ClusterUnlabeledEnumAlgorithm {
    INTEGER_PARTITION /**< One clique per part of each integer partition of n */
};

/**
 * @brief An enumerated cluster graph
 */
struct ClusterUnlabeledEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted with u < v) */
};

/**
 * @brief Result of non-isomorphic cluster enumeration
 */
struct ClusterUnlabeledEnumerationResult {
    std::vector<ClusterUnlabeledEnumeratedGraph> graphs;  /**< Array of enumerated graphs */
};

namespace detail {

/**
 * @brief Builds the disjoint union of cliques of the given sizes
 * @param n Number of vertices (must equal the sum of @p parts)
 * @param parts Clique sizes
 * @return ClusterUnlabeledEnumeratedGraph
 *
 * The parts occupy consecutive vertex blocks in order, and within a block the
 * pairs are generated in lexicographic order, so the edge list comes out
 * sorted with first < second without an extra sort.
 */
inline ClusterUnlabeledEnumeratedGraph cluster_unlabeled_build_graph(
    int n, const std::vector<int>& parts) {
    ClusterUnlabeledEnumeratedGraph g;
    g.n = n;
    int offset = 0;
    for (std::size_t p = 0; p < parts.size(); ++p) {
        int sz = parts[p];
        for (int i = 1; i <= sz; ++i) {
            for (int j = i + 1; j <= sz; ++j) {
                g.edges.push_back(std::make_pair(offset + i, offset + j));
            }
        }
        offset += sz;
    }
    return g;
}

/**
 * @brief Enumerates integer partitions and builds a graph for each partition
 *
 * Enumerates all partitions of @p remaining into positive parts of size at
 * most @p max_part, generated in non-increasing order so that each partition
 * is produced exactly once.
 *
 * @param remaining Number of vertices still to be covered
 * @param max_part Upper bound on the next part (keeps the parts non-increasing)
 * @param parts Partition under construction
 * @param results Storage for results
 * @param n Total number of vertices
 */
inline void cluster_unlabeled_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    std::vector<ClusterUnlabeledEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        results.push_back(cluster_unlabeled_build_graph(n, parts));
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        cluster_unlabeled_partition_dfs(remaining - s, s, parts, results, n);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic cluster graphs on n vertices
 * @param n Number of vertices
 * @param connected_only If true, emit only connected graphs
 * @param algo Algorithm to use (default: INTEGER_PARTITION)
 * @return ClusterUnlabeledEnumerationResult
 *
 * Emits one representative per isomorphism class: A000041(n) graphs
 * (1, 2, 3, 5, 7, 11, 15, 22, ... for n = 1, 2, ...). With @p connected_only
 * the output is the single complete graph K_n, the only connected cluster
 * graph on n vertices. n = 0 yields the single empty graph in both modes;
 * negative n yields nothing.
 *
 * @note The result materializes every graph; A000041 grows subexponentially
 *       (p(100) is about 1.9e8), but each graph carries up to n(n-1)/2 edges.
 */
inline ClusterUnlabeledEnumerationResult enumerate_cluster_unlabeled_graphs(
    int n, bool connected_only = false,
    ClusterUnlabeledEnumAlgorithm algo =
        ClusterUnlabeledEnumAlgorithm::INTEGER_PARTITION) {
    (void)algo;
    ClusterUnlabeledEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_cluster accepts the empty graph vacuously; emit it, matching
        // the empty partition counted by A000041(0) = 1.
        ClusterUnlabeledEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    if (connected_only) {
        // A cluster graph is connected iff it is a single clique.
        std::vector<int> parts(1, n);
        result.graphs.push_back(detail::cluster_unlabeled_build_graph(n, parts));
        return result;
    }

    std::vector<int> parts;
    detail::cluster_unlabeled_partition_dfs(n, n, parts, result.graphs, n);

    return result;
}

}  // namespace graph_recognition

#endif
