#ifndef GRAPH_RECOGNITION_POLYHEDRAL_ENUM_H
#define GRAPH_RECOGNITION_POLYHEDRAL_ENUM_H

/**
 * @file polyhedral_labeled_enum.h
 * @brief Polyhedral graph (3-connected planar graph) enumeration (reverse search)
 *
 * Enumerates all labeled polyhedral graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = removal of the vertex with the largest label from G
 *
 * Since triconnected is not hereditary, intermediate steps apply
 * the following pruning, and the final step performs full verification:
 *   1. Planarity pruning (hereditary: if non-planar, no extension is planar)
 *   2. Endgame connectivity pruning: with at most 2 vertices left to add,
 *      prune as soon as the current graph has 2 or more connected components.
 *      (Counting components earlier is unsound: a single future vertex may
 *      merge arbitrarily many components.)
 *   3. Degree lower bound pruning: check degree >= 3 constraint for each vertex at the last 3 levels
 *   4. Full triconnected + planarity check at the final step
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"
#include "recognizers/planar.h"
#include "recognizers/triconnected.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for polyhedral graph enumeration
 */
enum class PolyhedralLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of polyhedral graph enumeration
 */
struct PolyhedralLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated polyhedral graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct PolyhedralLabeledEnumState {
    int total_n;
    int alive_count;  /**< Active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    std::vector<int> deg;  /**< Degree of each vertex */

    explicit PolyhedralLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief Count connected components on {1, ..., x}
 */
inline int polyhedral_count_components(const PolyhedralLabeledEnumState& state,
                                       int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (int u = 1; u <= x; ++u) {
                if (!visited[u] && state.adj[v][u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
    }
    return comp;
}

/**
 * @brief DFS for polyhedral reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Prunes by planarity (hereditary) in intermediate steps,
 * and performs triconnected + planarity check at the final step.
 */
inline void polyhedral_labeled_enum_dfs(PolyhedralLabeledEnumState& state,
                                 std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // Final step: full planarity + triconnected check
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    edges.push_back(std::make_pair(u, v));

        Graph g(state.total_n, edges);
        PlanarResult pr = check_planar(g);
        if (!pr.is_planar) return;

        TriconnectedResult tr = check_triconnected(g);
        if (tr.is_triconnected) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            graph.edges = edges;
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;

    // For intermediate steps: pre-extract base edges
    std::vector<std::pair<int, int>> base_edges;
    for (int u = 1; u <= k; ++u)
        for (int v = u + 1; v <= k; ++v)
            if (state.adj[u][v])
                base_edges.push_back(std::make_pair(u, v));

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // Pruning 0: skip if new vertex x degree + remaining < 3
        int deg_x = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) {
                deg_x += (int)(tmp & 1);
                tmp >>= 1;
            }
        }
        if (deg_x + remaining < 3) continue;

        // Set edges from x to {1,...,k} based on mask
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) {
                state.deg[u]++;
            }
        }
        state.deg[x] = deg_x;
        state.alive_count = x;

        bool prune = false;

        // Pruning 1: planarity (hereditary)
        {
            std::vector<std::pair<int, int>> edges = base_edges;
            for (int u = 1; u <= k; ++u)
                if (state.adj[x][u])
                    edges.push_back(std::make_pair(u, x));
            Graph g(x, edges);
            PlanarResult pr = check_planar(g);
            if (!pr.is_planar) {
                prune = true;
            }
        }

        // Pruning 2: connectivity
        // A single future vertex may merge arbitrarily many components, so
        // only the endgame is decidable: with <= 2 vertices left, >= 2
        // components leave a cut set of size <= 2 (not 3-connected).
        if (!prune) {
            int comp = polyhedral_count_components(state, x);
            if (comp > 1 && remaining <= 2) {
                prune = true;
            }
        }

        // Pruning 3: degree lower bound (triconnected requires all vertices deg >= 3)
        if (!prune && remaining <= 2) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 3) {
                    prune = true;
                    break;
                }
            }
        }

        if (!prune) {
            polyhedral_labeled_enum_dfs(state, out);
        }

        // Restore
        for (int u = 1; u <= k; ++u) {
            if (state.adj[x][u]) {
                state.deg[u]--;
            }
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.deg[x] = 0;
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled polyhedral graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return PolyhedralLabeledEnumerationResult
 *
 * Uses reverse search. Since 3-connectivity is not hereditary,
 * intermediate steps apply planarity + connectivity + degree pruning,
 * and the final step performs full triconnected + planarity check.
 */
inline PolyhedralLabeledEnumerationResult
enumerate_polyhedral_labeled_graphs(int n,
    PolyhedralLabeledEnumAlgorithm algo =
        PolyhedralLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    PolyhedralLabeledEnumerationResult result;
    if (n < 4) return result;
    detail::PolyhedralLabeledEnumState root(n);
    detail::polyhedral_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
