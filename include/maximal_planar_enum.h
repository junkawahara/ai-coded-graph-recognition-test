#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H

/**
 * @file maximal_planar_enum.h
 * @brief Enumeration of maximal planar graphs (triangulations) (reverse search)
 *
 * Enumerates all labeled maximal planar graphs.
 *
 * Maximal planar graph: a planar graph where adding any edge destroys planarity.
 * For n >= 3, equivalent to edge count = 3n-6. All faces are triangles (triangulation).
 *
 * parent(G) = remove the vertex with the largest label from G
 * (planar is a hereditary class)
 *
 * Vertices are added in order 1, 2, ..., n, and at each step all possible
 * neighborhood subsets are enumerated and filtered by planar recognition.
 * Only outputs graphs with edge count = target_edges at the final step.
 *
 * Pruning: computes upper and lower bounds on the new vertex's degree at each step,
 * skipping unreachable neighborhood patterns.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"
#include "graph.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for maximal planar enumeration
 */
enum class MaximalPlanarEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of maximal planar enumeration
 */
struct MaximalPlanarEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated maximal planar graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct MaximalPlanarEnumState {
    int total_n;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    int target_edges; /**< Number of edges required for a maximal planar graph */

    explicit MaximalPlanarEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          target_edges(n >= 3 ? 3 * n - 6 : n * (n - 1) / 2) {}
};

/**
 * @brief DFS for maximal planar reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not planar,
 * Only outputs graphs with edge count = target_edges at the final step.
 */
inline void maximal_planar_enum_dfs(MaximalPlanarEnumState& state,
                                    std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        int edge_count = 0;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    ++edge_count;
        if (edge_count != state.target_edges) return;

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

    // Compute current edge count
    int current_edges = 0;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                ++current_edges;

    // Upper bound on edges addable after vertex x (excluding x)
    // remaining_capacity_after_x = sum_{j=x+1}^{n} (j-1)
    long long remaining_after_x = 0;
    for (int j = x + 1; j <= state.total_n; ++j)
        remaining_after_x += (j - 1);

    // Lower and upper bounds on the degree of new vertex x
    int d_min_raw = static_cast<int>(
        state.target_edges - current_edges - remaining_after_x);
    int d_min = (d_min_raw > 0) ? d_min_raw : 0;
    int d_max = state.target_edges - current_edges;
    if (d_max > k) d_max = k;

    // Prune if unreachable
    if (d_min > k || d_max < 0) return;

    // Pre-extract base edges
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // Pruning via popcount
        int deg = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) {
                tmp &= (tmp - 1);
                ++deg;
            }
        }
        if (deg < d_min || deg > d_max) continue;

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
        PlanarResult res = check_planar(g);

        if (res.is_planar) {
            maximal_planar_enum_dfs(state, out);
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
 * @brief Enumerates all labeled maximal planar graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return MaximalPlanarEnumerationResult
 *
 * Uses reverse search. parent(G) is obtained by removing the vertex
 * with the largest label from G.planar is a hereditary class,
 * so the property is preserved under any vertex removal.
 * Only outputs graphs with edge count = 3n-6 (n>=3) at the final step.
 */
inline MaximalPlanarEnumerationResult
enumerate_maximal_planar_graphs_reverse_search(int n,
    MaximalPlanarEnumAlgorithm algo =
        MaximalPlanarEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    MaximalPlanarEnumerationResult result;
    if (n < 0) return result;
    detail::MaximalPlanarEnumState root(n);
    detail::maximal_planar_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
