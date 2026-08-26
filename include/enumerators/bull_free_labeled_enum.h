#ifndef GRAPH_RECOGNITION_BULL_FREE_ENUM_H
#define GRAPH_RECOGNITION_BULL_FREE_ENUM_H

/**
 * @file bull_free_labeled_enum.h
 * @brief Bull-free graph enumeration (reverse search)
 *
 * Enumerates all labeled bull-free graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (bull-free is a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by bull-free test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "recognizers/bull_free.h"
#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for bull-free enumeration
 */
enum class BullFreeLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of bull-free enumeration
 */
struct BullFreeLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated bull-free graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct BullFreeLabeledEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit BullFreeLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for bull-free reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not bull-free.
 */
inline void bull_free_labeled_enum_dfs(BullFreeLabeledEnumState& state,
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
        BullFreeResult res = check_bull_free(g);

        if (res.is_bull_free) {
            bull_free_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled bull-free graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return BullFreeLabeledEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. Bull-free is a hereditary class,
 * so the property is preserved under any vertex removal.
 */
inline BullFreeLabeledEnumerationResult
enumerate_bull_free_labeled_graphs_reverse_search(int n,
    BullFreeLabeledEnumAlgorithm algo =
        BullFreeLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BullFreeLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::BullFreeLabeledEnumState root(n);
    detail::bull_free_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
