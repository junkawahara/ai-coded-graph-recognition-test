#ifndef GRAPH_RECOGNITION_PERMUTATION_ENUM_H
#define GRAPH_RECOGNITION_PERMUTATION_ENUM_H

/**
 * @file permutation_labeled_enum.h
 * @brief Permutation graph enumeration (reverse search)
 *
 * Enumerates all labeled permutation graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = removal of the vertex with the largest label from G
 * (always valid since permutation is a hereditary class)
 *
 * Vertices are added in the order 1, 2, ..., n, and at each step all possible
 * neighborhood subsets are enumerated and filtered by permutation recognition.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "recognizers/permutation.h"
#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for permutation graph enumeration
 */
enum class PermutationLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of permutation graph enumeration
 */
struct PermutationLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated permutation graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct PEnumState {
    int total_n;
    int alive_count;  /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit PEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for permutation reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count} as
 * its neighborhood. Prunes children that are not permutation graphs.
 */
inline void p_enum_dfs(PEnumState& state,
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
        PermutationResult res = check_permutation(g);

        if (res.is_permutation) {
            p_enum_dfs(state, out);
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
 * @brief Enumerates all labeled permutation graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return PermutationLabeledEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. Since permutation is a hereditary class,
 * the property is preserved under removal of any vertex.
 */
inline PermutationLabeledEnumerationResult
enumerate_permutation_labeled_graphs_reverse_search(int n,
    PermutationLabeledEnumAlgorithm algo =
        PermutationLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    PermutationLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::PEnumState root(n);
    detail::p_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
