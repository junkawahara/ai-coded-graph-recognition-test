#ifndef GRAPH_RECOGNITION_COGRAPH_ENUM_H
#define GRAPH_RECOGNITION_COGRAPH_ENUM_H

/**
 * @file cograph_enum.h
 * @brief Cograph (P4-free) enumeration (recursive cotree construction)
 *
 * Enumerates all labeled cographs on vertex set {1, ..., n}
 * by recursive cotree construction.
 *
 * A cograph is uniquely represented by a cotree:
 *   - Leaves correspond to vertices
 *   - Internal nodes have type 0 (disjoint union) or 1 (join)
 *   - Adjacent internal nodes have different types (alternating)
 *   - Each internal node has at least 2 children
 *
 * References:
 *   - Seinsche, 1974 (P4-free characterization)
 *   - Conte, Kante, Kurita, Uno, Wasa, DAM 2023 (proximity search)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for cograph enumeration
 */
enum class CographEnumAlgorithm {
    COTREE /**< recursive cotree construction */
};

/**
 * @brief Result of cograph enumeration
 */
struct CographEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated cographs */
};

namespace detail {

/**
 * @brief Recursive generation of set partitions
 *
 * Based on restricted growth strings. elems[0] is always placed in part 0.
 * Each subsequent element is placed in an existing part or a new part.
 * Only partitions with k >= 2 are output.
 */
inline void cograph_partition_dfs(
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

    // Add to an existing part
    for (int p = 0; p < num_parts; ++p) {
        parts[p].push_back(elems[idx]);
        cograph_partition_dfs(elems, idx + 1, parts, num_parts, out);
        parts[p].pop_back();
    }

    // Start a new part
    parts[num_parts].push_back(elems[idx]);
    cograph_partition_dfs(elems, idx + 1, parts, num_parts + 1, out);
    parts[num_parts].pop_back();
}

/**
 * @brief Generates all partitions of a set (k >= 2)
 */
inline std::vector<std::vector<std::vector<int>>>
generate_cograph_partitions(const std::vector<int>& elems) {
    std::vector<std::vector<std::vector<int>>> result;
    if (elems.size() < 2) return result;

    std::vector<std::vector<int>> parts(elems.size());
    parts[0].push_back(elems[0]);
    cograph_partition_dfs(elems, 1, parts, 1, &result);
    parts[0].pop_back();

    return result;
}

/**
 * @brief Recursive generation of Cartesian products
 *
 * groups[i] is the list of edge set candidates for the i-th part.
 * Generates all combinations by choosing one from each group.
 */
inline void cograph_cartesian_dfs(
    const std::vector<std::vector<std::vector<std::pair<int, int>>>>& groups,
    std::size_t group_idx,
    std::vector<const std::vector<std::pair<int, int>>*>& current,
    const std::vector<std::vector<int>>& partition,
    int type,
    std::vector<std::vector<std::pair<int, int>>>* out) {

    if (group_idx == groups.size()) {
        // Merge edges
        std::vector<std::pair<int, int>> merged;
        for (std::size_t i = 0; i < current.size(); ++i) {
            merged.insert(merged.end(), current[i]->begin(), current[i]->end());
        }

        // For join (type 1): add all edges between different parts
        if (type == 1) {
            int k = (int)partition.size();
            for (int p1 = 0; p1 < k; ++p1) {
                for (int p2 = p1 + 1; p2 < k; ++p2) {
                    for (std::size_t a = 0; a < partition[p1].size(); ++a) {
                        for (std::size_t b = 0; b < partition[p2].size(); ++b) {
                            int u = partition[p1][a];
                            int v = partition[p2][b];
                            if (u > v) std::swap(u, v);
                            merged.push_back(std::make_pair(u, v));
                        }
                    }
                }
            }
        }

        std::sort(merged.begin(), merged.end());
        out->push_back(merged);
        return;
    }

    for (std::size_t i = 0; i < groups[group_idx].size(); ++i) {
        current.push_back(&groups[group_idx][i]);
        cograph_cartesian_dfs(groups, group_idx + 1, current, partition,
                              type, out);
        current.pop_back();
    }
}

/**
 * @brief Enumerates cographs on the specified vertex set by cotree root type
 * @param vertices Vertex set
 * @param type 0 = union (disjoint union), 1 = join
 * @param edge_sets Output: edge set of each cograph
 *
 * In a canonical cotree, adjacent internal nodes alternate types,
 * so the opposite type is applied to children.
 */
inline void enumerate_cographs_by_type(
    const std::vector<int>& vertices, int type,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    if (vertices.size() <= 1) {
        // Leaf: no edges
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    // All partitions of the vertex set (k >= 2)
    std::vector<std::vector<std::vector<int>>> partitions =
        generate_cograph_partitions(vertices);

    for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
        const std::vector<std::vector<int>>& partition = partitions[pi];
        int k = (int)partition.size();

        // Recurse with opposite type for each part
        std::vector<std::vector<std::vector<std::pair<int, int>>>> part_results(k);
        for (int p = 0; p < k; ++p) {
            enumerate_cographs_by_type(partition[p], 1 - type, &part_results[p]);
        }

        // Generate all combinations via Cartesian product
        std::vector<const std::vector<std::pair<int, int>>*> current;
        cograph_cartesian_dfs(part_results, 0, current, partition, type,
                              edge_sets);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled cographs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only COTREE)
 * @return CographEnumerationResult
 *
 * Direct enumeration by recursive cotree construction.
 * For n >= 2, the results of root type 0 (union) and 1 (join) are combined.
 */
inline CographEnumerationResult enumerate_cograph_graphs_cotree(int n,
    CographEnumAlgorithm algo = CographEnumAlgorithm::COTREE) {
    (void)algo;
    CographEnumerationResult result;
    if (n < 0) return result;

    if (n == 0) {
        /* The empty graph is a cograph (check_cograph(n=0) is YES) */
        EnumeratedGraph g;
        g.n = 0;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<int> vertices;
    for (int i = 1; i <= n; ++i) vertices.push_back(i);

    // root type 0 (union) and 1 (join) are disjoint for n >= 2
    std::vector<std::vector<std::pair<int, int>>> type0;
    detail::enumerate_cographs_by_type(vertices, 0, &type0);

    std::vector<std::vector<std::pair<int, int>>> type1;
    detail::enumerate_cographs_by_type(vertices, 1, &type1);

    for (std::size_t i = 0; i < type0.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = type0[i];
        result.graphs.push_back(g);
    }
    for (std::size_t i = 0; i < type1.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = type1[i];
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
