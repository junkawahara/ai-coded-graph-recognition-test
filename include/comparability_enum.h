#ifndef GRAPH_RECOGNITION_COMPARABILITY_ENUM_H
#define GRAPH_RECOGNITION_COMPARABILITY_ENUM_H

/**
 * @file comparability_enum.h
 * @brief Comparability graph enumeration by labeled vertex extension
 *
 * Enumerates all labeled comparability graphs on vertex set {1, ..., n}
 * by adding vertices in label order.
 *
 * The public function and enum keep the historical "reverse_search" name, but
 * the implemented search tree is the labeled vertex-extension tree whose parent
 * removes the largest-labeled vertex. Comparability graphs are hereditary, so
 * every induced prefix of a target graph is comparability and the target is
 * reached exactly once.
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by comparability test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "comparability.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for comparability graph enumeration
 */
enum class ComparabilityEnumAlgorithm {
    REVERSE_SEARCH /**< legacy name for labeled vertex-extension search */
};

/**
 * @brief Result of comparability graph enumeration
 */
struct ComparabilityEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated comparability graphs */
};

namespace detail {

/** @brief Internal state for labeled vertex-extension search */
struct ComparabilityEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit ComparabilityEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for comparability graph vertex-extension search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not comparability.
 */
inline void comparability_enum_dfs(ComparabilityEnumState& state,
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
        ComparabilityResult res = check_comparability(g);

        if (res.is_comparability) {
            comparability_enum_dfs(state, out);
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
 * @brief Enumerates all labeled comparability graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return ComparabilityEnumerationResult
 *
 * The function name keeps the historical "reverse_search" suffix. The
 * implementation is a labeled vertex-extension search whose parent map removes
 * the largest-labeled vertex. Comparability graphs are hereditary, so the
 * property is preserved under any vertex removal.
 */
inline ComparabilityEnumerationResult
enumerate_comparability_graphs_reverse_search(int n,
    ComparabilityEnumAlgorithm algo =
        ComparabilityEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    ComparabilityEnumerationResult result;
    if (n < 0) return result;
    detail::ComparabilityEnumState root(n);
    detail::comparability_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
