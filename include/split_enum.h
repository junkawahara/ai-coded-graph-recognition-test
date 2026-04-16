#ifndef GRAPH_RECOGNITION_SPLIT_ENUM_H
#define GRAPH_RECOGNITION_SPLIT_ENUM_H

/**
 * @file split_enum.h
 * @brief Split graph enumeration (reverse search)
 *
 * Enumerates all labeled split graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * Split = chordal ∩ co-chordal. Since it is a hereditary class,
 * it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = removal of the simplicial vertex with the largest label from G
 * Additionally checks split property when generating children, pruning non-split children.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "split.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for split graph enumeration
 */
enum class SplitEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of split graph enumeration
 */
struct SplitEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated split graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalEnumState (for split)
 */
inline Graph split_state_to_graph(const ChordalEnumState& state) {
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
 * @brief DFS for split reverse search
 *
 * Same structure as chordal reverse search, but verifies split property
 * at each node and prunes non-split subtrees.
 */
inline void split_reverse_search_dfs(const ChordalEnumState& state,
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
        Graph g = split_state_to_graph(children[i]);
        SplitResult sr = check_split(g);
        if (!sr.is_split) continue;

        split_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled split graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return SplitEnumerationResult
 *
 * Extends chordal reverse search with split property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline SplitEnumerationResult enumerate_split_graphs_reverse_search(int n,
    SplitEnumAlgorithm algo = SplitEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    SplitEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::split_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
