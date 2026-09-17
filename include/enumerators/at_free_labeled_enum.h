#ifndef GRAPH_RECOGNITION_AT_FREE_ENUM_H
#define GRAPH_RECOGNITION_AT_FREE_ENUM_H

/**
 * @file at_free_labeled_enum.h
 * @brief AT-free graph enumeration (reverse search)
 *
 * Enumerates all labeled AT-free graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (AT-free graphs are a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by AT-free test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/at_free.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for AT-free graph enumeration
 */
enum class ATFreeLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of AT-free graph enumeration
 */
struct ATFreeLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated AT-free graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct ATFreeLabeledEnumState {
    int total_n;
    int alive_count; /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit ATFreeLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for AT-free graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not AT-free.
 */
inline void at_free_labeled_enum_dfs(ATFreeLabeledEnumState& state,
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
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

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
        ATFreeResult res = check_at_free(g);

        if (res.is_at_free) {
            at_free_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled AT-free graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return ATFreeLabeledEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. AT-free graphs are a hereditary
 * class, so the property is preserved under any vertex removal.
 * @throws std::invalid_argument if n < 0 or n > 64
 *         (the candidate neighbourhoods of a new vertex are 64-bit masks)
 */
inline ATFreeLabeledEnumerationResult
enumerate_at_free_labeled_graphs_reverse_search(int n,
    ATFreeLabeledEnumAlgorithm algo =
        ATFreeLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ATFreeLabeledEnumerationResult result;
    require_vertex_count("enumerate_at_free_labeled_graphs_reverse_search", n, 64);
    detail::ATFreeLabeledEnumState root(n);
    detail::at_free_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
