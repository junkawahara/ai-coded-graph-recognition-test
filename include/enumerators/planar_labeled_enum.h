#ifndef GRAPH_RECOGNITION_PLANAR_ENUM_H
#define GRAPH_RECOGNITION_PLANAR_ENUM_H

/**
 * @file planar_labeled_enum.h
 * @brief Labeled planar graph enumeration
 *
 * PLANTRI (default) deletes edges from embedded triangulations while
 * preserving connectivity, using canonical ear-addition parents. Abstract
 * isomorph rejection removes duplicate embeddings; component multisets
 * include disconnected graphs. Each representative is expanded into all
 * n!/|Aut(G)| labelings. No planarity recognizer is used on this path.
 *
 * REVERSE_SEARCH retains the original vertex additions and output order:
 * parent(G) = removal of the vertex with the largest label from G
 * (always valid since planar is a hereditary class)
 *
 * Vertices are added in the order 1, 2, ..., n, and at each step all
 * possible neighborhood subsets are enumerated and filtered by planar recognition.
 *
 * Reference: Brinkmann--McKay, "Fast generation of planar graphs,"
 * MATCH 58 (2007), section 4. No external plantri dependency; its in-place
 * updates, orbit pruning and throughput guarantees are not implemented.
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/planar.h"
#include "util/vertex_count.h"
#include "util/labeled_expansion.h"
#include "util/planar_edge_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for planar graph enumeration
 */
enum class PlanarLabeledEnumAlgorithm {
    REVERSE_SEARCH, /**< Legacy vertex additions with recognition */
    PLANTRI /**< Embedded edge deletions, component multisets and label expansion */
};

/**
 * @brief Result of planar graph enumeration
 */
struct PlanarLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated planar graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct PlanarLabeledEnumState {
    int total_n;
    int alive_count;  /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;

    explicit PlanarLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)) {}
};

/**
 * @brief DFS for planar reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not planar.
 */
inline void planar_labeled_enum_dfs(PlanarLabeledEnumState& state,
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
        PlanarResult res = check_planar(g);

        if (res.is_planar) {
            planar_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled planar graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (default: PLANTRI)
 * @return PlanarLabeledEnumerationResult
 *
 * PLANTRI stores abstract canonical forms and component representatives,
 * plus all outputs and the current label-expansion orbit. The embedded
 * search uses O(n^3) working space; abstract canonicalization has factorial
 * worst-case cost. Distinct embeddings are explored even if their abstract
 * graph has already been emitted. n = 0 yields one empty graph.
 * @throws std::invalid_argument if algo is invalid, n < 0, n > 63 for
 *         PLANTRI (abstract adjacency masks), or n > 64 for REVERSE_SEARCH
 *         (legacy candidate neighbourhood masks)
 */
inline PlanarLabeledEnumerationResult
enumerate_planar_labeled_graphs_reverse_search(int n,
    PlanarLabeledEnumAlgorithm algo =
        PlanarLabeledEnumAlgorithm::PLANTRI) {
    PlanarLabeledEnumerationResult result;
    if (algo != PlanarLabeledEnumAlgorithm::PLANTRI &&
        algo != PlanarLabeledEnumAlgorithm::REVERSE_SEARCH)
        throw std::invalid_argument("invalid planar labeled enumeration algorithm");
    require_vertex_count("enumerate_planar_labeled_graphs_reverse_search", n,
                        algo == PlanarLabeledEnumAlgorithm::PLANTRI ? 63 : 64);
    if (algo == PlanarLabeledEnumAlgorithm::PLANTRI) {
        detail::planar_edge_enumerate(n, false, [&](const std::vector<std::pair<int, int> >& edges) {
            detail::expand_graph_labelings(n, edges, &result.graphs);
        });
        return result;
    }
    detail::PlanarLabeledEnumState root(n);
    detail::planar_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
