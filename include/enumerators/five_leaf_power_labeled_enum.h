#ifndef GRAPH_RECOGNITION_FIVE_LEAF_POWER_ENUM_H
#define GRAPH_RECOGNITION_FIVE_LEAF_POWER_ENUM_H

/**
 * @file five_leaf_power_labeled_enum.h
 * @brief Enumeration of 5-leaf power graphs (reverse search)
 *
 * Enumerates all labeled 5-leaf power graphs.
 *
 * Since 5-leaf power is a subclass of Strongly Chordal (which is a subclass of Chordal)
 * and is a hereditary class, it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = remove the simplicial vertex with the largest label from G
 * Additionally checks 5-leaf power property when generating children, pruning non-5-leaf-power children.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "recognizers/five_leaf_power.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Result of 5-leaf power enumeration
 */
struct FiveLeafPowerLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated 5-leaf power graphs */
};

namespace detail {

/**
 * @brief Constructs a Graph from ChordalLabeledEnumState (for 5-leaf power)
 */
inline Graph fivlp_state_to_graph(const ChordalLabeledEnumState& state) {
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
 * @brief DFS for 5-leaf power reverse search
 *
 * Same structure as chordal reverse search, but verifies 5-leaf power
 * property at each node and prunes non-5-leaf-power subtrees.
 */
inline void five_leaf_power_reverse_search_dfs(const ChordalLabeledEnumState& state,
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
    std::vector<ChordalLabeledEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        // 5-leaf power check: prune if child is not 5-leaf power
        Graph g = fivlp_state_to_graph(children[i]);
        FiveLeafPowerResult fr = check_five_leaf_power(g);
        if (!fr.is_five_leaf_power) continue;

        five_leaf_power_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled 5-leaf power graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @return FiveLeafPowerLabeledEnumerationResult
 *
 * Extends chordal reverse search with 5-leaf power pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline FiveLeafPowerLabeledEnumerationResult enumerate_five_leaf_power_labeled_graphs_reverse_search(int n) {
    FiveLeafPowerLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalLabeledEnumState root(n);
    detail::five_leaf_power_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
