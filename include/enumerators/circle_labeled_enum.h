#ifndef GRAPH_RECOGNITION_CIRCLE_ENUM_H
#define GRAPH_RECOGNITION_CIRCLE_ENUM_H

/**
 * @file circle_labeled_enum.h
 * @brief Circle graph enumeration (reverse search)
 *
 * Enumerates all labeled circle graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (circle graphs are a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all
 * subsets of possible neighborhoods are enumerated and filtered by circle graph test.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "recognizers/circle.h"
#include "util/graph.h"

namespace graph_recognition {

enum class CircleLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

struct CircleLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated circle graphs */
};

namespace detail {

struct CircleLabeledEnumState {
    int total_n;
    int alive_count; /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit CircleLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)) {}
};

/**
 * @brief DFS for circle graph reverse search
 */
inline void circle_labeled_enum_dfs(CircleLabeledEnumState& state,
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
        CircleResult res = check_circle(g);

        if (res.is_circle) {
            circle_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled circle graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return CircleLabeledEnumerationResult
 */
inline CircleLabeledEnumerationResult
enumerate_circle_labeled_graphs_reverse_search(int n,
    CircleLabeledEnumAlgorithm algo = CircleLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CircleLabeledEnumerationResult result;
    if (n < 0) return result;
    detail::CircleLabeledEnumState root(n);
    detail::circle_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
