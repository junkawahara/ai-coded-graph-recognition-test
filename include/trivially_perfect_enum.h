#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_ENUM_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_ENUM_H

/**
 * @file trivially_perfect_enum.h
 * @brief Trivially perfect graph enumeration (UVD tree construction)
 *
 * Enumerates all labeled trivially perfect graphs on vertex set {1, ..., n}
 * via recursive construction of Universal Vertex Decomposition (UVD) trees.
 *
 * Trivially perfect = chordal ∩ cograph = {P4, C4}-free.
 * In a connected trivially perfect graph, the set U of universal vertices
 * is non-empty, and V \ U splits into 2 or more connected components.
 * Since this decomposition (UVD) is unique, constructive enumeration without duplicates is possible.
 *
 * References:
 *   - Golumbic, 1978 (trivially perfect characterization)
 *   - Galvin, Wesley, Zacovic, JIS 25, 2022 (labeled counting)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for trivially perfect graph enumeration
 */
enum class TriviallyPerfectEnumAlgorithm {
    UVD_TREE /**< Recursive construction of UVD tree */
};

/**
 * @brief Result of trivially perfect graph enumeration
 */
struct TriviallyPerfectEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated trivially perfect graphs */
};

namespace detail {

/**
 * @brief Recursive generation of set partitions (k >= 2)
 *
 * Based on restricted growth strings. elems[0] is always placed in part 0.
 * Each subsequent element is placed in an existing part or a new part.
 * Only outputs partitions with k >= 2 parts.
 */
inline void tp_partition_dfs(
    const std::vector<int>& elems, std::size_t idx,
    std::vector<std::vector<int>>& parts, int num_parts,
    std::vector<std::vector<std::vector<int>>>* out) {

    if (idx == elems.size()) {
        if (num_parts >= 2) {
            out->push_back(
                std::vector<std::vector<int>>(parts.begin(),
                                              parts.begin() + num_parts));
        }
        return;
    }

    // Add to existing part
    for (int p = 0; p < num_parts; ++p) {
        parts[p].push_back(elems[idx]);
        tp_partition_dfs(elems, idx + 1, parts, num_parts, out);
        parts[p].pop_back();
    }

    // Start a new part
    parts[num_parts].push_back(elems[idx]);
    tp_partition_dfs(elems, idx + 1, parts, num_parts + 1, out);
    parts[num_parts].pop_back();
}

/**
 * @brief Generate all partitions of a set (k >= 2)
 */
inline std::vector<std::vector<std::vector<int>>>
tp_generate_partitions_k2(const std::vector<int>& elems) {
    std::vector<std::vector<std::vector<int>>> result;
    if (elems.size() < 2) return result;

    std::vector<std::vector<int>> parts(elems.size());
    parts[0].push_back(elems[0]);
    tp_partition_dfs(elems, 1, parts, 1, &result);
    parts[0].pop_back();

    return result;
}

/**
 * @brief Recursive generation of Cartesian product
 *
 * groups[i] is the list of edge set candidates for the i-th part.
 * Generates all combinations by selecting one from each group.
 */
inline void tp_cartesian_dfs(
    const std::vector<std::vector<std::vector<std::pair<int, int>>>>& groups,
    std::size_t group_idx,
    std::vector<const std::vector<std::pair<int, int>>*>& current,
    std::vector<std::vector<std::pair<int, int>>>* out) {

    if (group_idx == groups.size()) {
        std::vector<std::pair<int, int>> merged;
        for (std::size_t i = 0; i < current.size(); ++i) {
            merged.insert(merged.end(), current[i]->begin(),
                          current[i]->end());
        }
        out->push_back(merged);
        return;
    }

    for (std::size_t i = 0; i < groups[group_idx].size(); ++i) {
        current.push_back(&groups[group_idx][i]);
        tp_cartesian_dfs(groups, group_idx + 1, current, out);
        current.pop_back();
    }
}

/**
 * @brief Enumerate connected trivially perfect graphs
 * @param vertices Vertex set
 * @param edge_sets Output: edge set of each graph
 *
 * Recursive construction based on UVD decomposition (no duplicates):
 *   1. Enumerate all non-empty subsets U of vertices as universal vertex set candidates
 *   2. If R = vertices \ U is empty, output the complete graph K_{|V|}
 *   3. If R is non-empty, partition R into k >= 2 parts and recurse on each part
 *   4. Edges = clique on U + all edges between U and R + recursive internal edges
 *
 * Duplicate avoidance: only output when U exactly matches the set of universal
 * vertices (vertices adjacent to all others) in the generated graph.
 * Due to the uniqueness of UVD decomposition, each graph is enumerated exactly once.
 */
inline void enumerate_connected_tp(
    const std::vector<int>& vertices,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    int sz = (int)vertices.size();

    // base case: single vertex
    if (sz == 1) {
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    if (sz >= 64) return;

    // Enumerate all non-empty subsets U via bitmask
    unsigned long long full = 1ULL << sz;
    for (unsigned long long mask = 1; mask < full; ++mask) {
        std::vector<int> U, R;
        for (int i = 0; i < sz; ++i) {
            if (mask & (1ULL << i)) {
                U.push_back(vertices[i]);
            } else {
                R.push_back(vertices[i]);
            }
        }

        // Pre-compute clique edges of U
        std::vector<std::pair<int, int>> u_clique;
        for (std::size_t a = 0; a < U.size(); ++a) {
            for (std::size_t b = a + 1; b < U.size(); ++b) {
                int x = U[a], y = U[b];
                if (x > y) std::swap(x, y);
                u_clique.push_back(std::make_pair(x, y));
            }
        }

        // Pre-compute all edges from U to R
        std::vector<std::pair<int, int>> u_to_r;
        for (std::size_t a = 0; a < U.size(); ++a) {
            for (std::size_t b = 0; b < R.size(); ++b) {
                int x = U[a], y = R[b];
                if (x > y) std::swap(x, y);
                u_to_r.push_back(std::make_pair(x, y));
            }
        }

        if (R.empty()) {
            // U = V: complete graph K_{|V|}
            // universal vertex = all vertices -> U = V matches exactly. Output it.
            std::vector<std::pair<int, int>> edges = u_clique;
            std::sort(edges.begin(), edges.end());
            edge_sets->push_back(edges);
            continue;
        }

        // Partition R into k >= 2 parts
        std::vector<std::vector<std::vector<int>>> partitions =
            tp_generate_partitions_k2(R);

        for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
            const std::vector<std::vector<int>>& partition = partitions[pi];
            int k = (int)partition.size();

            // Recursively enumerate connected TPs for each part
            std::vector<std::vector<std::vector<std::pair<int, int>>>>
                part_results(k);
            for (int p = 0; p < k; ++p) {
                enumerate_connected_tp(partition[p], &part_results[p]);
            }

            // Cartesian product
            std::vector<std::vector<std::pair<int, int>>> combined;
            std::vector<const std::vector<std::pair<int, int>>*> current;
            tp_cartesian_dfs(part_results, 0, current, &combined);

            // Add U clique edges + U-R edges to each combination
            for (std::size_t ci = 0; ci < combined.size(); ++ci) {
                std::vector<std::pair<int, int>> full_edges = combined[ci];
                full_edges.insert(full_edges.end(),
                                  u_clique.begin(), u_clique.end());
                full_edges.insert(full_edges.end(),
                                  u_to_r.begin(), u_to_r.end());

                // Duplicate avoidance: verify no vertex in R becomes a universal vertex.
                // Vertices in U are adjacent to all vertices (U clique + U-R all edges), so they are universal.
                // If vertex r in R were universal -> r is adjacent to all vertices -> r is adjacent to
                // vertices in other parts of R. But R is partitioned into k >= 2 parts and
                // no inter-part edges exist -> r is not adjacent to vertices in other parts.
                // Therefore no vertex in R can become universal, and
                // U exactly matches the universal vertex set.
                // Note: |R| == 1 with k >= 2 is not generated by tp_generate_partitions_k2,
                // so there is no issue.

                std::sort(full_edges.begin(), full_edges.end());
                edge_sets->push_back(full_edges);
            }
        }
    }
}

/**
 * @brief Enumerate all trivially perfect graphs (including disconnected ones)
 * @param vertices Vertex set
 * @param edge_sets Output: edge set of each graph
 *
 * Splits vertex set into connected components and takes Cartesian product of connected TPs for each.
 * Generates both k = 1 (connected) and k >= 2 (disconnected).
 */
inline void enumerate_all_tp(
    const std::vector<int>& vertices,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    if (vertices.empty()) {
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    // k = 1: connected TP graphs
    enumerate_connected_tp(vertices, edge_sets);

    // k >= 2: disconnected TP graphs (no edges between components)
    if (vertices.size() >= 2) {
        std::vector<std::vector<std::vector<int>>> partitions =
            tp_generate_partitions_k2(vertices);

        for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
            const std::vector<std::vector<int>>& partition = partitions[pi];
            int k = (int)partition.size();

            std::vector<std::vector<std::vector<std::pair<int, int>>>>
                part_results(k);
            for (int p = 0; p < k; ++p) {
                enumerate_connected_tp(partition[p], &part_results[p]);
            }

            std::vector<const std::vector<std::pair<int, int>>*> current;
            tp_cartesian_dfs(part_results, 0, current, edge_sets);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled trivially perfect graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only UVD_TREE)
 * @return TriviallyPerfectEnumerationResult
 *
 * Constructive enumeration via recursive UVD tree construction.
 * Component partition -> universal vertex set selection per component -> residual partition -> recurse.
 */
inline TriviallyPerfectEnumerationResult
enumerate_trivially_perfect_graphs_uvd(int n,
    TriviallyPerfectEnumAlgorithm algo =
        TriviallyPerfectEnumAlgorithm::UVD_TREE) {
    (void)algo;
    TriviallyPerfectEnumerationResult result;
    if (n <= 0) return result;

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<int> vertices;
    for (int i = 1; i <= n; ++i) vertices.push_back(i);

    std::vector<std::vector<std::pair<int, int>>> edge_sets;
    detail::enumerate_all_tp(vertices, &edge_sets);

    for (std::size_t i = 0; i < edge_sets.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = edge_sets[i];
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
