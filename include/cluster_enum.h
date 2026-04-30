#ifndef GRAPH_RECOGNITION_CLUSTER_ENUM_H
#define GRAPH_RECOGNITION_CLUSTER_ENUM_H

/**
 * @file cluster_enum.h
 * @brief Labeled enumeration of all cluster graphs (P3-free / disjoint union of cliques)
 *
 * Constructively enumerates all labeled cluster graphs on n vertices
 * via recursive set partition enumeration.
 * Since each set partition corresponds to a unique cluster graph,
 * deduplication is unnecessary. The enumeration count is the Bell number B(n) (OEIS A000110).
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Enumerated cluster graph
 */
struct ClusterEnumeratedGraph {
    int n;                                        /**< number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< edge list (sorted with u < v) */
};

/**
 * @brief Result of cluster graph enumeration
 */
struct ClusterEnumerationResult {
    std::vector<ClusterEnumeratedGraph> graphs;   /**< array of enumerated cluster graphs */
};

namespace detail_cluster_enum {

/**
 * @brief Builds a graph from a set partition (internal function)
 * @param n Number of vertices
 * @param blocks Partition blocks (each block is a set of vertices)
 * @return ClusterEnumeratedGraph
 *
 * Adds all vertex pairs within each block as edges (complete graph).
 */
inline ClusterEnumeratedGraph build_cluster_graph(
    int n,
    const std::vector<std::vector<int> >& blocks) {
    ClusterEnumeratedGraph g;
    g.n = n;
    for (size_t b = 0; b < blocks.size(); ++b) {
        const std::vector<int>& block = blocks[b];
        for (size_t i = 0; i < block.size(); ++i) {
            for (size_t j = i + 1; j < block.size(); ++j) {
                int u = block[i];
                int v = block[j];
                if (u > v) {
                    int tmp = u;
                    u = v;
                    v = tmp;
                }
                g.edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return g;
}

/**
 * @brief Recursive set partition enumeration (internal function)
 * @param vertex Current vertex to place (1-indexed)
 * @param n Number of vertices
 * @param blocks Current partition blocks
 * @param out Vector to store results
 *
 * Processes vertices from 1 to n in order, adding each vertex
 * to an existing block or creating a new block.
 */
inline void set_partition_dfs(
    int vertex,
    int n,
    std::vector<std::vector<int> >& blocks,
    std::vector<ClusterEnumeratedGraph>& out) {
    if (vertex > n) {
        out.push_back(build_cluster_graph(n, blocks));
        return;
    }

    // Add to an existing block
    size_t num_blocks = blocks.size();
    for (size_t i = 0; i < num_blocks; ++i) {
        blocks[i].push_back(vertex);
        set_partition_dfs(vertex + 1, n, blocks, out);
        blocks[i].pop_back();
    }

    // Create a new block
    std::vector<int> new_block;
    new_block.push_back(vertex);
    blocks.push_back(new_block);
    set_partition_dfs(vertex + 1, n, blocks, out);
    blocks.pop_back();
}

} // namespace detail_cluster_enum

/**
 * @brief Enumerates all labeled cluster graphs on n vertices
 * @param n Number of vertices
 * @return ClusterEnumerationResult
 *
 * Constructs B(n) graphs via recursive set partition enumeration.
 */
inline ClusterEnumerationResult enumerate_cluster_graphs(int n) {
    ClusterEnumerationResult result;
    if (n < 0) return result;
    if (n == 0) {
        // check_cluster accepts the empty graph vacuously; emit one empty
        // partition to match Bell(0) = 1.
        ClusterEnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<std::vector<int> > blocks;
    detail_cluster_enum::set_partition_dfs(1, n, blocks, result.graphs);

    return result;
}

} // namespace graph_recognition

#endif
