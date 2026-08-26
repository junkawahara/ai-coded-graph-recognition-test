#ifndef GRAPH_RECOGNITION_BLOCK_ENUM_H
#define GRAPH_RECOGNITION_BLOCK_ENUM_H

/**
 * @file block_labeled_enum.h
 * @brief Block graph enumeration (reverse search)
 *
 * Enumerates all labeled block graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * Block = chordal and every biconnected component is a clique (diamond-free chordal).
 * As a hereditary class, it can be enumerated as a subtree of the chordal reverse search tree.
 *
 * parent(G) = remove the simplicial vertex with the largest label from G
 * Block property is additionally checked during child generation, pruning non-block children.
 *
 * References:
 *   - Nakano, Uno, WALCOM 2020; ISAAC 2020 / Discrete Appl. Math. 2023
 *   - Hebert-Johnson, Lokshtanov, Vigoda, ESA 2023 (chordal enumeration)
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "block.h"
#include "chordal_labeled_enum.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for block graph enumeration
 */
enum class BlockLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of block graph enumeration
 */
struct BlockLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated block graphs */
};

namespace detail {

/**
 * @brief Constructs a Graph from ChordalLabeledEnumState
 */
inline Graph block_state_to_graph(const ChordalLabeledEnumState& state) {
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
 * @brief DFS for block graph reverse search
 *
 * Same structure as chordal reverse search, but verifies block property
 * at each node and prunes non-block subtrees.
 */
inline void block_reverse_search_dfs(const ChordalLabeledEnumState& state,
                                      std::vector<EnumeratedGraph>* out) {
    // All vertices alive -> completed graph
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
        // Block property check: prune if child is not block
        Graph g = block_state_to_graph(children[i]);
        BlockResult br = check_block(g);
        if (!br.is_block) continue;

        block_reverse_search_dfs(children[i], out);
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled block graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return BlockLabeledEnumerationResult
 *
 * Extends chordal reverse search with block property pruning.
 * parent(G) is obtained by removing the simplicial vertex with the largest label from G.
 */
inline BlockLabeledEnumerationResult enumerate_block_labeled_graphs_reverse_search(int n,
    BlockLabeledEnumAlgorithm algo = BlockLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BlockLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::ChordalLabeledEnumState root(n);
    detail::block_reverse_search_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
