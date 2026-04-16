#ifndef GRAPH_RECOGNITION_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_INTERVAL_ENUM_H

/**
 * @file interval_enum.h
 * @brief Enumeration of interval graphs (reverse search)
 *
 * Enumerates all labeled interval graphs.
 *
 * Interval = chordal ∩ AT-free, and since it is a hereditary class,
 * it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = remove the simplicial vertex with the largest label from G
 * When generating children, Interval property is additionally checked, and non- Interval children are pruned.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "interval.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for interval enumeration
 */
enum class IntervalEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of interval enumeration
 */
struct IntervalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated interval graphs */
};

namespace detail {

/**
 * @brief Constructs a Graph from ChordalEnumState (for interval)
 */
inline Graph interval_state_to_graph(const ChordalEnumState& state) {
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
 * @brief DFS for interval reverse search
 *
 * Same structure as chordal reverse search, but verifies interval property at each node
 * and prunes non-interval subtrees.
 */
inline void interval_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = interval_state_to_graph(children[i]);
        IntervalResult ir = check_interval(g);
        if (!ir.is_interval) continue;

        interval_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled Interval graphon vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return IntervalEnumerationResult
 *
 * Extends chordal reverse search with interval property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline IntervalEnumerationResult enumerate_interval_graphs_reverse_search(int n,
    IntervalEnumAlgorithm algo = IntervalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    IntervalEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::interval_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
