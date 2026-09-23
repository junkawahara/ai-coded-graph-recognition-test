#ifndef GRAPH_RECOGNITION_CO_INTERVAL_ENUM_H
#define GRAPH_RECOGNITION_CO_INTERVAL_ENUM_H

/**
 * @file co_interval_labeled_enum.h
 * @brief Co-interval graph enumeration by complementing interval graphs
 *
 * Enumerates all labeled co-interval graphs on vertex set {1, ..., n}
 * by complementing the output of the interval enumerator by Kiyomi, Kijima,
 * and Uno. Complementation is a bijection on labeled graphs, so no additional
 * recognition filter or duplicate elimination is needed.
 *
 * The previous vertex-addition reverse search remains selectable as
 * REVERSE_SEARCH: parent(G) removes the largest-label vertex, and each
 * possible neighborhood of the next vertex is filtered by recognition.
 *
 * Reference: M. Kiyomi, S. Kijima, and T. Uno, "Listing Chordal Graphs and
 * Interval Graphs," WG 2006, LNCS 4271, 68--77, 2006.
 * DOI: 10.1007/11917496_7
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/interval_labeled_enum.h"
#include "recognizers/co_interval.h"
#include "util/graph.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-interval graph enumeration
 */
enum class CoIntervalLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous largest-label vertex search with recognition */
    KIYOMI_KIJIMA_UNO = 1 /**< Complement of dedicated interval enumeration (default) */
};

/**
 * @brief Result of co-interval graph enumeration
 */
struct CoIntervalLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated co-interval graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct CoIntervalLabeledEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit CoIntervalLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for co-interval graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not co-interval.
 */
inline void co_interval_labeled_enum_dfs(CoIntervalLabeledEnumState& state,
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
        CoIntervalResult res = check_co_interval(g);

        if (res.is_co_interval) {
            co_interval_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled co-interval graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection; KIYOMI_KIJIMA_UNO is the default
 * @return CoIntervalLabeledEnumerationResult
 *
 * KIYOMI_KIJIMA_UNO complements each interval graph as it is generated,
 * preserving the labels {1, ..., n}, including isolated vertices. Only the
 * co-interval results are retained. Complement construction adds O(n^2) time
 * per output to the existing interval enumerator; the original paper's
 * per-output time bound does not carry over to this implementation.
 * REVERSE_SEARCH retains the original vertex-addition search and its order.
 * n = 0 yields the single empty graph. Both choices retain the existing
 * supported range 0..64 (the legacy search uses 64-bit neighborhood masks).
 * @throws std::invalid_argument if n < 0 or n > 64
 *         or the algorithm selector is unknown
 */
inline CoIntervalLabeledEnumerationResult
enumerate_co_interval_labeled_graphs_reverse_search(int n,
    CoIntervalLabeledEnumAlgorithm algo =
        CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO) {
    CoIntervalLabeledEnumerationResult result;
    require_vertex_count("enumerate_co_interval_labeled_graphs_reverse_search", n, 64);
    if (algo == CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH) {
        detail::CoIntervalLabeledEnumState root(n);
        detail::co_interval_labeled_enum_dfs(root, &result.graphs);
    } else if (algo == CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO) {
        enumerate_interval_labeled_graphs_reverse_search_cb(
            n, [&result](const EnumeratedGraph& interval) {
                EnumeratedGraph graph;
                graph.n = interval.n;
                // Interval edge lists are sorted with u < v. Merge against
                // all vertex pairs to emit the sorted complement directly.
                std::size_t edge_index = 0;
                for (int u = 1; u <= interval.n; ++u) {
                    for (int v = u + 1; v <= interval.n; ++v) {
                        const std::pair<int, int> edge(u, v);
                        if (edge_index < interval.edges.size() &&
                            interval.edges[edge_index] == edge) {
                            ++edge_index;
                        } else {
                            graph.edges.push_back(edge);
                        }
                    }
                }
                result.graphs.push_back(std::move(graph));
            }, IntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO);
    } else {
        throw std::invalid_argument(
            "enumerate_co_interval_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
