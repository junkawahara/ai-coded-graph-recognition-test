#ifndef GRAPH_RECOGNITION_BIPARTITE_ENUM_H
#define GRAPH_RECOGNITION_BIPARTITE_ENUM_H

/**
 * @file bipartite_labeled_enum.h
 * @brief Bipartite graph enumeration by color-compatible vertex additions
 *
 * Enumerates all labeled bipartite graphs on vertex set {1, ..., n}
 * using the two color classes of each current connected component.
 *
 * parent(G) = remove the vertex with the largest label from G
 * (bipartite graphs are a hereditary class, so this is always valid)
 *
 * Vertices are added in order 1, 2, ..., n. A neighborhood may meet only
 * one color class per component, with the empty choice generated once.
 * This directly generates all valid children without recognition calls.
 * REVERSE_SEARCH retains the previous all-subsets search with recognition.
 */

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <vector>

#include "recognizers/bipartite.h"
#include "enumerators/chordal_labeled_enum.h"
#include "util/bipartite_neighborhoods.h"
#include "util/graph.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for bipartite graph enumeration
 */
enum class BipartiteLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Previous all-neighborhoods search with recognition */
    BIPARTITE_COLORING = 1 /**< Generate only color-compatible neighborhoods (default) */
};

/**
 * @brief Result of bipartite graph enumeration
 */
struct BipartiteLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated bipartite graphs */
};

namespace detail {

/** @brief Adds the next label using only neighborhoods that preserve bipartiteness. */
inline void bipartite_labeled_coloring_dfs(
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

    const std::vector<BipartiteComponentSides> sides = bipartite_component_sides(k, adj);
    enumerate_bipartite_neighborhoods(sides, [&](unsigned long long neighbors) {
        adj.push_back(neighbors);
        for (int u = 0; u < k; ++u)
            if ((neighbors >> u) & 1ULL) adj[u] |= 1ULL << k;

        bipartite_labeled_coloring_dfs(k + 1, adj, n, out);

        for (int u = 0; u < k; ++u)
            if ((neighbors >> u) & 1ULL) adj[u] &= ~(1ULL << k);
        adj.pop_back();
    });
}

/** @brief Internal state for reverse search */
struct BipartiteLabeledEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit BipartiteLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for bipartite graph reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not bipartite.
 */
inline void bipartite_labeled_enum_dfs(BipartiteLabeledEnumState& state,
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
        BipartiteResult res = check_bipartite(g);

        if (res.is_bipartite) {
            bipartite_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled bipartite graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: BIPARTITE_COLORING)
 * @return BipartiteLabeledEnumerationResult
 *
 * Both algorithms remove the largest label to obtain the unique parent.
 * BIPARTITE_COLORING generates the Cartesian product of the permitted
 * component neighborhoods; REVERSE_SEARCH scans all subsets and recognizes
 * each child, preserving its previous output order. Orders can differ.
 * All results are kept in memory. n = 0 yields one empty graph.
 * The historical function name is retained for source compatibility.
 * @throws std::invalid_argument if n < 0, n > 64 (64-bit masks), or algo is unknown
 */
inline BipartiteLabeledEnumerationResult
enumerate_bipartite_labeled_graphs_reverse_search(int n,
    BipartiteLabeledEnumAlgorithm algo =
        BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING) {
    BipartiteLabeledEnumerationResult result;
    require_vertex_count("enumerate_bipartite_labeled_graphs_reverse_search", n, 64);
    switch (algo) {
        case BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING: {
            std::vector<unsigned long long> adj;
            detail::bipartite_labeled_coloring_dfs(0, adj, n, result.graphs);
            break;
        }
        case BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH: {
            detail::BipartiteLabeledEnumState root(n);
            detail::bipartite_labeled_enum_dfs(root, &result.graphs);
            break;
        }
        default:
            throw std::invalid_argument(
                "enumerate_bipartite_labeled_graphs_reverse_search: unknown algorithm");
    }
    return result;
}

}  // namespace graph_recognition

#endif
