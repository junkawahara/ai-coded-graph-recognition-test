#ifndef GRAPH_RECOGNITION_CO_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_CO_CHORDAL_ENUM_H

/**
 * @file co_chordal_labeled_enum.h
 * @brief Co-chordal graph enumeration by complementing chordal graphs
 *
 * Enumerates all labeled co-chordal graphs on vertex set {1, ..., n}
 * by complementing the output of the chordal enumerator by Kiyomi and Uno.
 * Complementation is a bijection on labeled graphs, so no recognition
 * filter or additional duplicate elimination is needed.
 *
 * The previous vertex-addition reverse search remains selectable as
 * REVERSE_SEARCH: parent(G) removes the largest-label vertex, and each
 * possible neighborhood of the next vertex is filtered by recognition.
 *
 * Reference: M. Kiyomi and T. Uno, "Generating Chordal Graphs Included
 * in Given Graphs," IEICE Trans. Inf. & Syst. E89-D(2), 763--770, 2006.
 * DOI: 10.1093/ietisy/e89-d.2.763
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "recognizers/co_chordal.h"
#include "util/graph.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-chordal graph enumeration
 */
enum class CoChordalLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous largest-label vertex search with recognition */
    KIYOMI_UNO = 1 /**< Complement of the chordal enumeration by Kiyomi and Uno (default) */
};

/**
 * @brief Result of co-chordal graph enumeration
 */
struct CoChordalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated co-chordal graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct CoChordalLabeledEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit CoChordalLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for co-chordal graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not co-chordal.
 */
inline void co_chordal_labeled_enum_dfs(CoChordalLabeledEnumState& state,
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
    // 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    // pre-extract base edges (between vertices 1..k, excluding x)
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
        CoChordalResult res = check_co_chordal(g);

        if (res.is_co_chordal) {
            co_chordal_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled co-chordal graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection; KIYOMI_UNO is the default
 * @return CoChordalLabeledEnumerationResult
 *
 * KIYOMI_UNO complements each chordal graph as it is generated, preserving
 * the labels {1, ..., n}, including isolated vertices. Only the co-chordal
 * results are retained. Complement construction costs O(n^2) per output;
 * the paper's constant-time difference-output bounds do not apply here.
 * REVERSE_SEARCH retains the original vertex-addition search and its order.
 * n = 0 yields the single empty graph. Both choices retain the existing
 * supported range 0..64 (the legacy search uses 64-bit neighborhood masks).
 * @throws std::invalid_argument if n < 0 or n > 64
 *         or the algorithm selector is unknown
 */
inline CoChordalLabeledEnumerationResult
enumerate_co_chordal_labeled_graphs_reverse_search(int n,
    CoChordalLabeledEnumAlgorithm algo =
        CoChordalLabeledEnumAlgorithm::KIYOMI_UNO) {
    CoChordalLabeledEnumerationResult result;
    require_vertex_count("enumerate_co_chordal_labeled_graphs_reverse_search", n, 64);
    if (algo == CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH) {
        detail::CoChordalLabeledEnumState root(n);
        detail::co_chordal_labeled_enum_dfs(root, &result.graphs);
    } else if (algo == CoChordalLabeledEnumAlgorithm::KIYOMI_UNO) {
        enumerate_chordal_labeled_graphs_reverse_search_cb(
            n, [&result](const EnumeratedGraph& chordal) {
                EnumeratedGraph graph;
                graph.n = chordal.n;
                // Chordal edge lists are sorted with u < v. Merge against
                // all vertex pairs to emit the sorted complement directly.
                std::size_t edge_index = 0;
                for (int u = 1; u <= chordal.n; ++u) {
                    for (int v = u + 1; v <= chordal.n; ++v) {
                        const std::pair<int, int> edge(u, v);
                        if (edge_index < chordal.edges.size() &&
                            chordal.edges[edge_index] == edge) {
                            ++edge_index;
                        } else {
                            graph.edges.push_back(edge);
                        }
                    }
                }
                result.graphs.push_back(std::move(graph));
            }, ChordalLabeledEnumAlgorithm::KIYOMI_UNO);
    } else {
        throw std::invalid_argument(
            "enumerate_co_chordal_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
