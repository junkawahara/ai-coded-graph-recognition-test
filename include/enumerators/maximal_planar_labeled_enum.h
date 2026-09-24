#ifndef GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H
#define GRAPH_RECOGNITION_MAXIMAL_PLANAR_ENUM_H

/**
 * @file maximal_planar_labeled_enum.h
 * @brief Enumeration of labeled maximal planar graphs (triangulations)
 *
 * Enumerates all labeled maximal planar graphs.
 *
 * Maximal planar graph: a planar graph where adding any edge destroys planarity.
 * For n >= 3, equivalent to edge count = 3n-6. All faces are triangles (triangulation).
 *
 * PLANTRI (default) uses the E3/E4/E5 local expansions of Brinkmann--McKay
 * (2007), with canonical reductions on embedded triangulations. Each
 * non-isomorphic representative is expanded into all distinct labelings.
 * The implementation uses rotation-system copies and sibling sets, not
 * the optimized plantri program; its throughput is not claimed here.
 *
 * REVERSE_SEARCH retains the previous algorithm:
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
#include <stdexcept>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "util/labeled_expansion.h"
#include "util/maximal_planar_expansion.h"
#include "recognizers/planar.h"
#include "util/vertex_count.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for maximal planar enumeration
 */
enum class MaximalPlanarLabeledEnumAlgorithm {
    REVERSE_SEARCH = 0, /**< Legacy vertex additions and planarity pruning */
    PLANTRI = 1 /**< E3/E4/E5 triangulation expansions and label expansion (default) */
};

/**
 * @brief Result of maximal planar enumeration
 */
struct MaximalPlanarLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated maximal planar graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct MaximalPlanarLabeledEnumState {
    int total_n;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    int target_edges; /**< Number of edges required for a maximal planar graph */

    explicit MaximalPlanarLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(static_cast<size_t>(n) + 1, std::vector<char>(static_cast<size_t>(n) + 1, 0)),
          target_edges(n >= 3 ? 3 * n - 6 : n * (n - 1) / 2) {}
};

/**
 * @brief DFS for maximal planar reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes children that are not planar,
 * Only outputs graphs with edge count = target_edges at the final step.
 */
inline void maximal_planar_labeled_enum_dfs(MaximalPlanarLabeledEnumState& state,
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
            maximal_planar_labeled_enum_dfs(state, out);
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
 * @param algo Algorithm selection (default: PLANTRI)
 * @return MaximalPlanarLabeledEnumerationResult
 *
 * PLANTRI constructs triangulations without recognition and expands each
 * representative to n!/|Aut(G)| labelings. REVERSE_SEARCH keeps the legacy
 * output order. All output is retained; label expansion also keeps the
 * visited edge sets of the current representative. The historical function
 * name is retained for source compatibility. n = 0..3 each yields K_n.
 * @throws std::invalid_argument if n < 0, n > 64, or algo is unknown
 *         (the legacy neighbourhood masks determine the API's range)
 */
inline MaximalPlanarLabeledEnumerationResult
enumerate_maximal_planar_labeled_graphs_reverse_search(int n,
    MaximalPlanarLabeledEnumAlgorithm algo =
        MaximalPlanarLabeledEnumAlgorithm::PLANTRI) {
    MaximalPlanarLabeledEnumerationResult result;
    require_vertex_count("enumerate_maximal_planar_labeled_graphs_reverse_search", n, 64);
    if (algo == MaximalPlanarLabeledEnumAlgorithm::PLANTRI) {
        detail::maximal_planar_expansion_enumerate(n,
            [&](const std::vector<std::pair<int, int> >& edges) {
                detail::expand_graph_labelings(n, edges, &result.graphs);
            });
        return result;
    }
    if (algo != MaximalPlanarLabeledEnumAlgorithm::REVERSE_SEARCH)
        throw std::invalid_argument("unknown maximal planar labeled enumeration algorithm");
    detail::MaximalPlanarLabeledEnumState root(n);
    detail::maximal_planar_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
