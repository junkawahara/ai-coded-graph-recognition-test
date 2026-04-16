#ifndef GRAPH_RECOGNITION_UNICYCLIC_ENUM_H
#define GRAPH_RECOGNITION_UNICYCLIC_ENUM_H

/**
 * @file unicyclic_enum.h
 * @brief Non-isomorphic unicyclic graph enumeration
 *
 * Enumerates all non-isomorphic unicyclic graphs on n vertices via constructive enumeration.
 * A unicyclic graph is connected with edge count = vertex count (contains exactly one cycle).
 *
 * Algorithm:
 *   Fix cycle length g (3 <= g <= n) and attach non-isomorphic rooted trees to each cycle vertex.
 *   Using bracelet canonical form considering the dihedral symmetry (rotation + reflection)
 *   of the cycle, only non-isomorphic graphs are output.
 *
 * Rooted tree generation reuses compute_rooted_trees from tree_enum.h.
 *
 * Non-isomorphic count: OEIS A001429
 *   0, 0, 1, 2, 5, 13, 33, 89, 240, 657, 1806, ...
 *
 * References:
 *   Harary, Palmer, "Graphical Enumeration,"
 *   Academic Press, 1973
 */

#include "tree_enum.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Enumerated unicyclic graph
 */
struct UnicyclicEnumeratedGraph {
    int n;                                        /**< Number of vertices */
    std::vector<std::pair<int, int> > edges;      /**< Edge list (sorted by u < v) */
};

/**
 * @brief Result of unicyclic graph enumeration
 */
struct UnicyclicEnumerationResult {
    std::vector<UnicyclicEnumeratedGraph> graphs;
};

namespace detail {

/**
 * @brief Bracelet canonicity check
 *
 * Determines whether the sequence seq is the lexicographically smallest
 * among all 2g transformations of the dihedral group D_g (rotation + reflection).
 */
inline bool is_canonical_bracelet(const std::vector<int>& seq) {
    int g = (int)seq.size();
    for (int start = 1; start < g; ++start) {
        // Rotation by start
        for (int i = 0; i < g; ++i) {
            int a = seq[i];
            int b = seq[(i + start) % g];
            if (a < b) break;
            if (a > b) return false;
        }
    }
    for (int start = 0; start < g; ++start) {
        // Reflection + rotation by start
        for (int i = 0; i < g; ++i) {
            int a = seq[i];
            int b = seq[((g - i) + start) % g];
            if (a < b) break;
            if (a > b) return false;
        }
    }
    return true;
}

/**
 * @brief Build edges to attach to cycle vertex from level sequence of rooted tree
 *
 * @param L Level sequence (L[0]=0 is the root)
 * @param cycle_vertex Vertex number on the cycle (1-indexed)
 * @param vertex_offset Starting number - 1 for additional vertices
 * @param edges Output edge list
 */
inline void attach_rooted_tree(
    const std::vector<int>& L,
    int cycle_vertex,
    int vertex_offset,
    std::vector<std::pair<int, int> >& edges) {

    int s = (int)L.size();
    if (s <= 1) return;  // Root only -> no additional edges

    std::vector<int> depth_last(s, 0);
    depth_last[0] = cycle_vertex;

    for (int j = 1; j < s; ++j) {
        int actual_vertex = vertex_offset + j;
        int parent_vertex = depth_last[L[j] - 1];
        int u = (parent_vertex < actual_vertex) ? parent_vertex : actual_vertex;
        int v = (parent_vertex < actual_vertex) ? actual_vertex : parent_vertex;
        edges.push_back(std::make_pair(u, v));
        depth_last[L[j]] = actual_vertex;
    }
}

/**
 * @brief Rooted tree ID table
 *
 * Structure that assigns global IDs to rooted trees of all sizes.
 * IDs are in ascending size order, lexicographic level sequence order within same size.
 */
struct RootedTreeTable {
    /** id_offset[s] = global ID of the first rooted tree of size s */
    std::vector<int> id_offset;
    /** Rooted tree cache (from tree_enum.h) */
    std::map<int, std::vector<std::vector<int> > > cache;
    /** Maximum size */
    int max_size;

    void build(int n) {
        max_size = n;
        for (int k = 1; k <= n; ++k) {
            compute_rooted_trees(k, cache);
        }
        id_offset.resize(n + 1, 0);
        int running = 0;
        for (int k = 1; k <= n; ++k) {
            id_offset[k] = running;
            running += (int)cache[k].size();
        }
    }

    int global_id(int size, int index) const {
        return id_offset[size] + index;
    }

    int tree_count(int size) const {
        std::map<int, std::vector<std::vector<int> > >::const_iterator it =
            cache.find(size);
        if (it == cache.end()) return 0;
        return (int)it->second.size();
    }

    const std::vector<int>& level_seq(int size, int index) const {
        return cache.find(size)->second[index];
    }
};

/**
 * @brief Enumerate rooted tree assignments to each cycle vertex via DFS
 */
inline void unicyclic_enum_dfs(
    int pos, int remaining, int g,
    std::vector<int>& sizes,
    std::vector<int>& indices,
    std::vector<int>& id_seq,
    const RootedTreeTable& table,
    std::vector<UnicyclicEnumeratedGraph>& results,
    int n) {

    if (pos == g) {
        if (remaining != 0) return;
        if (!is_canonical_bracelet(id_seq)) return;

        // Graph construction
        UnicyclicEnumeratedGraph graph;
        graph.n = n;

        // Cycle edges
        for (int i = 1; i < g; ++i) {
            graph.edges.push_back(std::make_pair(i, i + 1));
        }
        graph.edges.push_back(std::make_pair(1, g));

        // Attach rooted tree to each cycle vertex
        int vertex_offset = g;
        for (int i = 0; i < g; ++i) {
            int cycle_vertex = i + 1;
            const std::vector<int>& L = table.level_seq(sizes[i], indices[i]);
            attach_rooted_tree(L, cycle_vertex, vertex_offset, graph.edges);
            vertex_offset += sizes[i] - 1;
        }

        std::sort(graph.edges.begin(), graph.edges.end());
        results.push_back(graph);
        return;
    }

    int max_extra = remaining;
    // Maximum size = max_extra + 1
    int max_size = max_extra + 1;
    if (max_size > n) max_size = n;

    for (int s = 1; s <= max_size; ++s) {
        int count = table.tree_count(s);
        for (int idx = 0; idx < count; ++idx) {
            sizes[pos] = s;
            indices[pos] = idx;
            id_seq[pos] = table.global_id(s, idx);
            unicyclic_enum_dfs(
                pos + 1, remaining - (s - 1), g,
                sizes, indices, id_seq, table, results, n);
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic unicyclic graphs on n vertices
 * @param n Number of vertices
 * @return UnicyclicEnumerationResult
 *
 * Scans cycle length g from 3 to n and constructively assigns
 * rooted trees to each cycle vertex. Using bracelet canonical form
 * considering the dihedral symmetry of the cycle, only non-isomorphic graphs are output.
 */
inline UnicyclicEnumerationResult enumerate_unicyclic_graphs(int n) {
    UnicyclicEnumerationResult result;
    if (n < 3) return result;

    // Build rooted tree table
    detail::RootedTreeTable table;
    table.build(n);

    for (int g = 3; g <= n; ++g) {
        int r = n - g;  // Additional vertices
        std::vector<int> sizes(g, 0);
        std::vector<int> indices(g, 0);
        std::vector<int> id_seq(g, 0);
        detail::unicyclic_enum_dfs(
            0, r, g, sizes, indices, id_seq, table, result.graphs, n);
    }

    return result;
}

}  // namespace graph_recognition

#endif
