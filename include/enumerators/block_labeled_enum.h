#ifndef GRAPH_RECOGNITION_BLOCK_ENUM_H
#define GRAPH_RECOGNITION_BLOCK_ENUM_H

/**
 * @file block_labeled_enum.h
 * @brief Labeled block graphs from weighted block trees and label expansion
 *
 * Enumerates all labeled block graphs on vertex set {1, ..., n}
 * using BlockGraphGen representatives and their distinct labelings.
 *
 * Block = chordal and every biconnected component is a clique (diamond-free chordal).
 * BLOCK_GRAPH_GEN constructs weighted block trees without recognition,
 * composes disconnected components, and expands each representative into
 * n!/|Aut(G)| labeled graphs. REVERSE_SEARCH retains the previous subtree
 * of the chordal reverse search tree, including its output order.
 *
 * parent(G) = remove the simplicial vertex with the largest label from G
 * Block property is additionally checked during child generation, pruning non-block children.
 *
 * References:
 *   - Ekim, Shalom, Yirik, "Generation of weighted trees, block trees and
 *     block graphs," 2024, https://arxiv.org/abs/2401.09764
 *   - Avis, Fukuda, "Reverse search for enumeration," 1996
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "recognizers/block.h"
#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/block_unlabeled_enum.h"
#include "util/graph.h"
#include "util/labeled_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for block graph enumeration
 */
enum class BlockLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous chordal search with block recognition */
    BLOCK_GRAPH_GEN = 1 /**< Weighted block trees and label expansion (default) */
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
 * @param algo Algorithm selection (default: BLOCK_GRAPH_GEN)
 * @return BlockLabeledEnumerationResult
 *
 * The default caches rooted block trees, connected representatives and the
 * current representative's visited labelings. All output is retained. Tree
 * generation, explicit clique edges, and label expansion have separate costs;
 * the paper's linear delay for tree representations is not claimed here.
 * REVERSE_SEARCH retains the old recognition-pruned chordal search.
 * n = 0 yields one empty graph. Algorithms may use different output orders.
 * The historical function name is retained for source compatibility.
 * @throws std::invalid_argument if n < 0, n > max_vertex_count
 *         (see util/vertex_count.h), or algo is unknown
 */
inline BlockLabeledEnumerationResult enumerate_block_labeled_graphs_reverse_search(int n,
    BlockLabeledEnumAlgorithm algo = BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN) {
    BlockLabeledEnumerationResult result;
    require_vertex_count("enumerate_block_labeled_graphs_reverse_search", n);
    switch (algo) {
        case BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN: {
            const BlockUnlabeledEnumerationResult representatives =
                enumerate_block_unlabeled_graphs(n);
            for (std::size_t i = 0; i < representatives.graphs.size(); ++i)
                detail::expand_graph_labelings(n, representatives.graphs[i].edges, &result.graphs);
            break;
        }
        case BlockLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::ChordalLabeledEnumState root(n);
            detail::block_reverse_search_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_block_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
