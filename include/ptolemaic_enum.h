#ifndef GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H
#define GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H

/**
 * @file ptolemaic_enum.h
 * @brief Ptolemaic graph enumeration (reverse search)
 *
 * Enumerates all labeled Ptolemaic graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * Ptolemaic = chordal ∩ distance-hereditary. Since it is a hereditary class,
 * it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = removal of the simplicial vertex with the largest label from G
 * Additionally checks Ptolemaic property when generating children, pruning non-Ptolemaic children.
 *
 * References:
 *   - Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023
 *   - Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (chordal enumeration)
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "ptolemaic.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Ptolemaic graph enumeration
 */
enum class PtolemaicEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of Ptolemaic graph enumeration
 */
struct PtolemaicEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated Ptolemaic graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalEnumState
 */
inline Graph state_to_graph(const ChordalEnumState& state) {
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
 * @brief DFS for Ptolemaic reverse search
 *
 * Same structure as chordal reverse search, but verifies Ptolemaic property
 * at each node and prunes non-Ptolemaic subtrees.
 */
inline void ptolemaic_reverse_search_dfs(const ChordalEnumState& state,
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
        // Ptolemaic check: prune if child is not Ptolemaic
        Graph g = state_to_graph(children[i]);
        PtolemaicResult pr = check_ptolemaic(g);
        if (!pr.is_ptolemaic) continue;

        ptolemaic_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled Ptolemaic graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return PtolemaicEnumerationResult
 *
 * Extends chordal reverse search with Ptolemaic property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline PtolemaicEnumerationResult enumerate_ptolemaic_graphs_reverse_search(int n,
    PtolemaicEnumAlgorithm algo = PtolemaicEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    PtolemaicEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalEnumState root(n);
    detail::ptolemaic_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
