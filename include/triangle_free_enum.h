#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H

/**
 * @file triangle_free_enum.h
 * @brief Triangle-free graph enumeration (reverse search)
 *
 * Enumerates all labeled triangle-free graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = removal of the vertex with the largest label from G
 * (always valid since triangle-free is a hereditary class)
 *
 * Vertices are added in the order 1, 2, ..., n, and at each step all
 * possible neighborhood subsets are enumerated and filtered by triangle-free recognition.
 *
 * References:
 *   - McKay, J. Algorithms 26, 1998 (geng -t: canonical augmentation)
 *   - Colbourn, Read, J. Graph Theory 3, 1979
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "triangle_free.h"
#include "graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for triangle-free graph enumeration
 */
enum class TriangleFreeEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of triangle-free graph enumeration
 */
struct TriangleFreeEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated triangle-free graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct TriangleFreeEnumState {
    int total_n;
    int alive_count;  /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit TriangleFreeEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for triangle-free reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not triangle-free.
 */
inline void triangle_free_enum_dfs(TriangleFreeEnumState& state,
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
        TriangleFreeResult res = check_triangle_free(g);

        if (res.is_triangle_free) {
            triangle_free_enum_dfs(state, out);
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
 * @brief Enumerates all labeled triangle-free graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return TriangleFreeEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. Since triangle-free is a hereditary class,
 * the property is preserved under removal of any vertex.
 */
inline TriangleFreeEnumerationResult
enumerate_triangle_free_graphs_reverse_search(int n,
    TriangleFreeEnumAlgorithm algo =
        TriangleFreeEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    TriangleFreeEnumerationResult result;
    if (n < 0) return result;
    detail::TriangleFreeEnumState root(n);
    detail::triangle_free_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
