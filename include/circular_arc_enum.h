#ifndef GRAPH_RECOGNITION_CIRCULAR_ARC_ENUM_H
#define GRAPH_RECOGNITION_CIRCULAR_ARC_ENUM_H

/**
 * @file circular_arc_enum.h
 * @brief Circular-arc graph enumeration (reverse search)
 *
 * Enumerates all labeled circular-arc graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (circular-arc graphs are a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by circular-arc test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "circular_arc.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for circular-arc graph enumeration
 */
enum class CircularArcEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of circular-arc graph enumeration
 */
struct CircularArcEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated circular-arc graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct CircularArcEnumState {
    int total_n;
    int alive_count; /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit CircularArcEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for circular-arc graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not circular-arc.
 */
inline void circular_arc_enum_dfs(CircularArcEnumState& state,
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
        CircularArcResult res = check_circular_arc(g);

        if (res.is_circular_arc) {
            circular_arc_enum_dfs(state, out);
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
 * @brief Enumerates all labeled circular-arc graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return CircularArcEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G. Circular-arc graphs are a hereditary
 * class, so the property is preserved under any vertex removal.
 */
inline CircularArcEnumerationResult
enumerate_circular_arc_graphs_reverse_search(int n,
    CircularArcEnumAlgorithm algo =
        CircularArcEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CircularArcEnumerationResult result;
    if (n < 0) return result;
    detail::CircularArcEnumState root(n);
    detail::circular_arc_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
