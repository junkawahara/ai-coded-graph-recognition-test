#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_ENUM_H

/**
 * @file three_leaf_power_enum.h
 * @brief 3-leaf power graph enumeration (reverse search)
 *
 * Enumerates all labeled 3-leaf power graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * 3-leaf power is a subset of Ptolemaic which is a subset of Chordal. Since it is a hereditary class,
 * it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = removal of the simplicial vertex with the largest label from G
 * Additionally checks 3-leaf power property when generating children, pruning non-3-leaf-power children.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "three_leaf_power.h"

namespace graph_recognition {

/**
 * @brief Result of 3-leaf power graph enumeration
 */
struct ThreeLeafPowerEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated 3-leaf power graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalEnumState
 */
inline Graph tlp_state_to_graph(const ChordalEnumState& state) {
    // Remap alive vertices to [1..alive_count] to avoid dead vertex overhead
    std::vector<int> remap(state.total_n + 1, 0);
    int cnt = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) remap[v] = ++cnt;
    }
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= state.total_n; ++u) {
        if (!state.alive[u]) continue;
        for (int v = u + 1; v <= state.total_n; ++v) {
            if (!state.alive[v]) continue;
            if (state.adj[u][v]) {
                edges.push_back(std::make_pair(remap[u], remap[v]));
            }
        }
    }
    return Graph(cnt, edges);
}

/**
 * @brief DFS for 3-leaf power reverse search
 *
 * Same structure as chordal reverse search, but verifies 3-leaf power property
 * at each node and prunes non-3-leaf-power subtrees.
 */
inline void three_leaf_power_reverse_search_dfs(const ChordalEnumState& state,
                                                 std::vector<EnumeratedGraph>* out) {
    // All vertices are alive -> completed graph
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    // Generate children (same as chordal reverse search)
    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        // 3-leaf power check: prune if child is not a 3-leaf power
        Graph g = tlp_state_to_graph(children[i]);
        ThreeLeafPowerResult tr = check_three_leaf_power(g);
        if (!tr.is_three_leaf_power) continue;

        three_leaf_power_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled 3-leaf power graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @return ThreeLeafPowerEnumerationResult
 *
 * Extends chordal reverse search with 3-leaf power property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline ThreeLeafPowerEnumerationResult enumerate_three_leaf_power_graphs_reverse_search(int n) {
    ThreeLeafPowerEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::three_leaf_power_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
