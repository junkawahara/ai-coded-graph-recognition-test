#ifndef GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H
#define GRAPH_RECOGNITION_PTOLEMAIC_ENUM_H

/**
 * @file ptolemaic_labeled_enum.h
 * @brief Labeled Ptolemaic enumeration by chordality-preserving pendant/twin extensions
 *
 * Enumerates all labeled Ptolemaic graphs on vertex set {1, ..., n} by
 * expanding the representatives from the structural unlabeled enumerator.
 * Connected representatives use pendant and true-twin additions, and false
 * twins only at simplicial vertices, so every extension preserves chordality
 * and distance heredity. Disconnected representatives are composed as
 * multisets of connected components. Adjacent label swaps generate every
 * distinct labeling, deduplicated by edge set, without recognition calls.
 *
 * A removable pendant/twin need not have the largest label. Expanding each
 * representative into n!/|Aut(G)| labelings avoids the omissions that would
 * result from restricting the extensions to increasing labels.
 *
 * REVERSE_SEARCH retains the previous chordal search with Ptolemaic
 * recognition. Its parent removes the simplicial vertex with largest label.
 *
 * References: Bandelt and Mulder, "Distance-hereditary graphs," 1986;
 * Yamazaki, Qian and Uehara, "Efficient enumeration of non-isomorphic
 * distance-hereditary graphs and related graphs," Discrete Appl. Math. 342,
 * 2024, DOI:10.1016/j.dam.2023.09.002. The specialized canonical-parent
 * search and the paper's delay guarantee are not implemented here.
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/ptolemaic_unlabeled_enum.h"
#include "util/graph.h"
#include "recognizers/ptolemaic.h"
#include "util/labeled_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Ptolemaic graph enumeration
 */
enum class PtolemaicLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous chordal search with Ptolemaic recognition */
    VERTEX_INCREMENTAL = 1 /**< Chordality-preserving pendant/twin extensions + label expansion (default) */
};

/**
 * @brief Result of Ptolemaic graph enumeration
 */
struct PtolemaicLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated Ptolemaic graphs */
};

namespace detail {

/**
 * @brief Build a Graph from ChordalLabeledEnumState
 */
inline Graph state_to_graph(const ChordalLabeledEnumState& state) {
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
inline void ptolemaic_reverse_search_dfs(const ChordalLabeledEnumState& state,
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
 * @param algo Algorithm selection (default: VERTEX_INCREMENTAL)
 * @return PtolemaicLabeledEnumerationResult
 *
 * VERTEX_INCREMENTAL reuses the structural unlabeled generator, including
 * disconnected graphs, and expands each representative's distinct labelings.
 * It retains canonical-form sets and connected representatives by size, plus
 * the visited labelings of the current representative. All output graphs are
 * materialized in memory. REVERSE_SEARCH preserves the previous algorithm
 * and its output order. The algorithms can enumerate in different orders.
 * n = 0 yields one empty graph. The historical function name is retained
 * for source compatibility.
 * @throws std::invalid_argument if n < 0, n > 63 for VERTEX_INCREMENTAL
 *         (the unlabeled generator's bitmask limit), n > max_vertex_count
 *         for REVERSE_SEARCH (see util/vertex_count.h), or algo is unknown
 */
inline PtolemaicLabeledEnumerationResult enumerate_ptolemaic_labeled_graphs_reverse_search(int n,
    PtolemaicLabeledEnumAlgorithm algo = PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    PtolemaicLabeledEnumerationResult result;
    require_vertex_count("enumerate_ptolemaic_labeled_graphs_reverse_search", n);
    switch (algo) {
        case PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL: {
            require_vertex_count("enumerate_ptolemaic_labeled_graphs_reverse_search", n, 63);
            const PtolemaicUnlabeledEnumerationResult representatives =
                enumerate_ptolemaic_unlabeled_graphs(n);
            for (std::size_t i = 0; i < representatives.graphs.size(); ++i) {
                detail::expand_graph_labelings(n, representatives.graphs[i].edges,
                                              &result.graphs);
            }
            break;
        }
        case PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::ChordalLabeledEnumState root(n);
            detail::ptolemaic_reverse_search_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_ptolemaic_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
