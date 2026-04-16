#ifndef GRAPH_RECOGNITION_CO_CHORDAL_ENUM_H
#define GRAPH_RECOGNITION_CO_CHORDAL_ENUM_H

/**
 * @file co_chordal_enum.h
 * @brief Co-chordal graph enumeration (reverse search)
 *
 * Enumerates all labeled co-chordal graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (co-chordal graphs are a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by co-chordal test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "co_chordal.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for co-chordal graph enumeration
 */
enum class CoChordalEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of co-chordal graph enumeration
 */
struct CoChordalEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated co-chordal graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct CoChordalEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit CoChordalEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for co-chordal graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not co-chordal.
 */
inline void co_chordal_enum_dfs(CoChordalEnumState& state,
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
            co_chordal_enum_dfs(state, out);
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
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return CoChordalEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. Co-chordal graphs are a hereditary
 * class, so the property is preserved under any vertex removal.
 */
inline CoChordalEnumerationResult
enumerate_co_chordal_graphs_reverse_search(int n,
    CoChordalEnumAlgorithm algo =
        CoChordalEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CoChordalEnumerationResult result;
    if (n < 0) return result;
    detail::CoChordalEnumState root(n);
    detail::co_chordal_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
