#ifndef GRAPH_RECOGNITION_DISTANCE_HEREDITARY_ENUM_H
#define GRAPH_RECOGNITION_DISTANCE_HEREDITARY_ENUM_H

/**
 * @file distance_hereditary_labeled_enum.h
 * @brief Labeled distance-hereditary enumeration by pendant/twin extensions
 *
 * Enumerates all labeled distance-hereditary graphs on the vertex set
 * {1, ..., n} by expanding the representatives from the unlabeled enumerator.
 * Connected representatives use pendant, true-twin and false-twin additions
 * with canonical-form sets per size. Disconnected representatives are composed
 * as multisets of connected components. Adjacent label swaps then generate
 * all distinct labelings, deduplicated by edge set, with no recognition calls.
 *
 * A removable pendant/twin need not have the largest label: restricting the
 * extensions to increasing labels alone would miss graphs. Label expansion
 * instead emits n!/|Aut(G)| graphs from each non-isomorphic representative.
 *
 * REVERSE_SEARCH retains the previous largest-label vertex-addition search,
 * filtering all neighborhood subsets by distance-hereditary recognition.
 *
 * References: Bandelt and Mulder, "Distance-hereditary graphs," 1986;
 * Yamazaki, Qian and Uehara, "Efficient enumeration of non-isomorphic
 * distance-hereditary graphs and related graphs," Discrete Appl. Math. 342,
 * 2024, DOI:10.1016/j.dam.2023.09.002. The paper's DH-tree parent search and
 * O(n^3) delay for graphs with at most n vertices are not implemented here.
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "enumerators/distance_hereditary_unlabeled_enum.h"
#include "recognizers/distance_hereditary.h"
#include "util/graph.h"
#include "util/labeled_expansion.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for distance-hereditary enumeration
 */
enum class DistanceHereditaryLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous vertex-addition search with recognition */
    VERTEX_INCREMENTAL = 1 /**< Pendant/twin extensions plus label expansion (default) */
};

/**
 * @brief Result of distance-hereditary enumeration
 */
struct DistanceHereditaryLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated distance-hereditary graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct DHEnumState {
    int total_n;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit DHEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for distance-hereditary reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not distance-hereditary.
 */
inline void dh_enum_dfs(DHEnumState& state,
                        std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    // L10: silently stops enumeration; 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    // M23: pre-extract base edges (between vertices 1..k, excluding x)
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
        }
        state.alive_count = x;

        std::vector<std::pair<int, int>> edges = base_edges;
        for (int u = 1; u <= k; ++u)
            if (state.adj[x][u])
                edges.push_back(std::make_pair(u, x));
        Graph g(x, edges);
        DistanceHereditaryResult res = check_distance_hereditary(g);

        if (res.is_distance_hereditary) {
            dh_enum_dfs(state, out);
        }

        for (int u = 1; u <= k; ++u) {
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled distance-hereditary graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: VERTEX_INCREMENTAL)
 * @return DistanceHereditaryLabeledEnumerationResult
 *
 * VERTEX_INCREMENTAL reuses the structural unlabeled generator, including
 * disconnected graphs, and expands each representative's distinct labelings.
 * It retains connected representatives by size and the visited labelings of
 * the current representative. All output graphs are materialized in memory.
 * REVERSE_SEARCH preserves the previous algorithm and its output order.
 * The algorithms can enumerate in different orders. n = 0 yields one empty
 * graph. The historical function name is retained for source compatibility.
 * @throws std::invalid_argument if n < 0, n > 63 for VERTEX_INCREMENTAL,
 *         n > 64 for REVERSE_SEARCH (the respective bitmask limits),
 *         or algo is unknown
 */
inline DistanceHereditaryLabeledEnumerationResult
enumerate_distance_hereditary_labeled_graphs_reverse_search(int n,
    DistanceHereditaryLabeledEnumAlgorithm algo =
        DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL) {
    DistanceHereditaryLabeledEnumerationResult result;
    require_vertex_count("enumerate_distance_hereditary_labeled_graphs_reverse_search", n, 64);
    switch (algo) {
        case DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL: {
            require_vertex_count("enumerate_distance_hereditary_labeled_graphs_reverse_search", n, 63);
            const DistanceHereditaryUnlabeledEnumerationResult representatives =
                enumerate_distance_hereditary_unlabeled_graphs(n);
            for (std::size_t i = 0; i < representatives.graphs.size(); ++i) {
                detail::expand_graph_labelings(n, representatives.graphs[i].edges,
                                              &result.graphs);
            }
            break;
        }
        case DistanceHereditaryLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::DHEnumState root(n);
            detail::dh_enum_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_distance_hereditary_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
