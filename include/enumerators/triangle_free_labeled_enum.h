#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_ENUM_H

/**
 * @file triangle_free_labeled_enum.h
 * @brief Triangle-free graph enumeration by independent-set vertex additions
 *
 * Enumerates all labeled triangle-free graphs on vertex set {1, ..., n}
 * using independent sets as the neighborhoods of new vertices.
 *
 * parent(G) = removal of the vertex with the largest label from G
 * (always valid since triangle-free is a hereditary class)
 *
 * Vertices are added in the order 1, 2, ..., n, and at each step all
 * independent neighborhoods are generated directly, without recognition.
 * REVERSE_SEARCH retains the previous all-subsets scan with recognition.
 *
 * References:
 *   - McKay, J. Algorithms 26, 1998 (geng -t: canonical augmentation)
 *   - Colbourn, Read, J. Graph Theory 3, 1979
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "recognizers/triangle_free.h"
#include "util/graph.h"
#include "util/independent_neighborhoods.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for triangle-free graph enumeration
 */
enum class TriangleFreeLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous all-neighborhoods search with recognition */
    INDEPENDENT_SETS = 1 /**< Generate only independent neighborhoods (default) */
};

/**
 * @brief Result of triangle-free graph enumeration
 */
struct TriangleFreeLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated triangle-free graphs */
};

namespace detail {

/** @brief Adds the next label using only independent neighborhoods. */
inline void triangle_free_labeled_independent_sets_dfs(
    int k, std::vector<unsigned long long>& adj, int n,
    std::vector<EnumeratedGraph>& out) {
    if (k == n) {
        EnumeratedGraph graph;
        graph.n = n;
        for (int u = 0; u < n; ++u)
            for (int v = u + 1; v < n; ++v)
                if ((adj[u] >> v) & 1ULL)
                    graph.edges.push_back(std::make_pair(u + 1, v + 1));
        out.push_back(std::move(graph));
        return;
    }

    enumerate_independent_neighborhoods(k, adj, [&](unsigned long long neighbors) {
        adj.push_back(neighbors);
        for (int u = 0; u < k; ++u)
            if ((neighbors >> u) & 1ULL) adj[u] |= 1ULL << k;

        triangle_free_labeled_independent_sets_dfs(k + 1, adj, n, out);

        for (int u = 0; u < k; ++u)
            if ((neighbors >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        adj.pop_back();
    });
}

/** @brief Internal state for reverse search */
struct TriangleFreeLabeledEnumState {
    int total_n;
    int alive_count;  /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit TriangleFreeLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for triangle-free reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not triangle-free.
 */
inline void triangle_free_labeled_enum_dfs(TriangleFreeLabeledEnumState& state,
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
            triangle_free_labeled_enum_dfs(state, out);
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
 * @param algo Algorithm selection (default: INDEPENDENT_SETS)
 * @return TriangleFreeLabeledEnumerationResult
 *
 * Both algorithms remove the largest label to obtain the unique parent.
 * Adding a vertex preserves triangle-freeness exactly when its neighborhood
 * is independent. INDEPENDENT_SETS generates these subsets directly;
 * REVERSE_SEARCH scans all subsets and recognizes each child. Both emit the
 * same graphs in the same order. All results are kept in memory; n = 0 yields
 * one empty graph. The historical function name is retained for compatibility.
 * @throws std::invalid_argument if n < 0, n > 64 (64-bit masks), or algo is unknown
 */
inline TriangleFreeLabeledEnumerationResult
enumerate_triangle_free_labeled_graphs_reverse_search(int n,
    TriangleFreeLabeledEnumAlgorithm algo =
        TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS) {
    TriangleFreeLabeledEnumerationResult result;
    require_vertex_count("enumerate_triangle_free_labeled_graphs_reverse_search", n, 64);
    switch (algo) {
        case TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS: {
            std::vector<unsigned long long> adj;
            detail::triangle_free_labeled_independent_sets_dfs(0, adj, n, result.graphs);
            break;
        }
        case TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::TriangleFreeLabeledEnumState root(n);
            detail::triangle_free_labeled_enum_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_triangle_free_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
