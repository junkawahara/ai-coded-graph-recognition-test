#ifndef GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_PROPER_INTERVAL_ENUM_H

/**
 * @file proper_interval_enum.h
 * @brief Proper interval graph enumeration (reverse search)
 *
 * Enumerates all labeled proper interval graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * Proper interval = interval ∩ claw-free. Since it is a hereditary class,
 * it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = removal of the simplicial vertex with the largest label from G
 * Additionally checks proper interval property when generating children, pruning non-proper-interval children.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "proper_interval.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for proper interval graph enumeration
 */
enum class ProperIntervalEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of proper interval graph enumeration
 */
struct ProperIntervalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated proper interval graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalEnumState (for proper interval)
 */
inline Graph proper_interval_state_to_graph(const ChordalEnumState& state) {
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
 * @brief DFS for proper interval reverse search
 *
 * Same structure as chordal reverse search, but verifies proper interval property
 * at each node and prunes non-proper-interval subtrees.
 */
inline void proper_interval_reverse_search_dfs(const ChordalEnumState& state,
                                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<ChordalEnumState> children;
    collect_children_reverse_search(state, &children);

    for (std::size_t i = 0; i < children.size(); ++i) {
        Graph g = proper_interval_state_to_graph(children[i]);
        ProperIntervalResult pr = check_proper_interval(g);
        if (!pr.is_proper_interval) continue;

        proper_interval_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled proper interval graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return ProperIntervalEnumerationResult
 *
 * Extends chordal reverse search with proper interval property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline ProperIntervalEnumerationResult enumerate_proper_interval_graphs_reverse_search(int n,
    ProperIntervalEnumAlgorithm algo = ProperIntervalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ProperIntervalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::proper_interval_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
