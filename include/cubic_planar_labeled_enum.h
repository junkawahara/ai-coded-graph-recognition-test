#ifndef GRAPH_RECOGNITION_CUBIC_PLANAR_ENUM_H
#define GRAPH_RECOGNITION_CUBIC_PLANAR_ENUM_H

/**
 * @file cubic_planar_labeled_enum.h
 * @brief Cubic planar graph enumeration (reverse search)
 *
 * Enumerates all labeled cubic planar graphs on vertex set {1, ..., n}.
 *
 * Cubic planar graph: a planar graph where every vertex has degree exactly 3.
 * No cubic graph exists if n is odd or n < 4.
 *
 * Algorithm:
 *   Vertices are added in order 1, 2, ..., n. When adding vertex x,
 *   neighbors are chosen from vertices in {1,...,x-1} with deg < 3.
 *   Pruning is performed using degree upper bounds (deg <= 3) and
 *   planarity (hereditary property). After all vertices are added,
 *   the graph is output if all degrees == 3 and the graph is planar.
 *
 * Planarity pruning:
 *   - x <= 5: skip (always planar for max-deg-3 graphs)
 *   - 6 <= x <= total_n-3: edge count upper bound only (m <= 3x-6)
 *   - x >= total_n-2: full planarity check (K5/K3,3 minor detection)
 *   This avoids the cost of Graph construction at intermediate steps
 *   while efficiently pruning non-planar graphs near the end.
 *
 * References:
 *   Brinkmann, McKay, "Fast generation of planar graphs,"
 *   MATCH Commun. Math. Comput. Chem. 58, 2007
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_labeled_enum.h"
#include "graph.h"
#include "planar.h"

namespace graph_recognition {

enum class CubicPlanarLabeledEnumAlgorithm {
    REVERSE_SEARCH
};

struct CubicPlanarLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

struct CubicPlanarLabeledEnumState {
    int total_n;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;
    int edge_count;

    explicit CubicPlanarLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0), edge_count(0) {}
};

inline bool cubic_planar_check_planar(const CubicPlanarLabeledEnumState& state, int x) {
    // Edge count fast check: m <= 3x - 6
    if (x >= 3 && state.edge_count > 3 * x - 6) return false;

    // Full planarity check via K5/K3,3 minor detection
    std::vector<std::pair<int, int> > edges;
    edges.reserve(state.edge_count);
    for (int u = 1; u <= x; ++u)
        for (int v = u + 1; v <= x; ++v)
            if (state.adj[u][v])
                edges.push_back(std::make_pair(u, v));
    Graph g(x, edges);
    PlanarResult pr = check_planar(g);
    return pr.is_planar;
}

inline void cubic_planar_labeled_enum_choose(CubicPlanarLabeledEnumState& state,
                                      const std::vector<int>& available,
                                      std::size_t start,
                                      int chosen_count,
                                      int min_size, int max_size,
                                      std::vector<EnumeratedGraph>* out);

inline void cubic_planar_labeled_enum_dfs(CubicPlanarLabeledEnumState& state,
                                   std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.deg[v] != 3) return;
        }
        // Final planarity check
        if (!cubic_planar_check_planar(state, state.total_n)) return;

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
    int remaining = state.total_n - x;

    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < 3) {
            available.push_back(v);
        }
    }

    int min_neighbors = 3 - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = 3;
    if (max_neighbors > (int)available.size()) max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    // The final graph must have exactly 3n/2 edges (cubic).
    int target_edges = 3 * state.total_n / 2;
    int max_future_edges = 3 * (state.total_n - x);
    if (state.edge_count + max_neighbors + max_future_edges < target_edges) return;

    cubic_planar_labeled_enum_choose(state, available, 0, 0,
                              min_neighbors, max_neighbors, out);
}

inline void cubic_planar_labeled_enum_choose(CubicPlanarLabeledEnumState& state,
                                      const std::vector<int>& available,
                                      std::size_t start,
                                      int chosen_count,
                                      int min_size, int max_size,
                                      std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_n - x;

    if (chosen_count >= min_size) {
        state.deg[x] = chosen_count;
        state.alive_count = x;

        bool feasible = true;
        for (int v = 1; v <= x; ++v) {
            if (state.deg[v] + remaining_after_x < 3) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            // Planarity pruning strategy:
            // - x <= 5: always planar for degree-3 graphs
            // - 6 <= x: edge count check (m <= 3x-6) - cheap
            // - x >= total_n - 2: full minor check - expensive but near end
            bool planar = true;
            if (x >= 6 && x >= 3 && state.edge_count > 3 * x - 6) {
                planar = false;
            } else if (x >= state.total_n - 2 && x >= 6) {
                planar = cubic_planar_check_planar(state, x);
            }

            if (planar) {
                cubic_planar_labeled_enum_dfs(state, out);
            }
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        if (state.deg[v] >= 3) continue;

        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;
        state.edge_count++;

        cubic_planar_labeled_enum_choose(state, available, i + 1, chosen_count + 1,
                                  min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
        state.edge_count--;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled cubic planar graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return CubicPlanarLabeledEnumerationResult
 */
inline CubicPlanarLabeledEnumerationResult
enumerate_cubic_planar_labeled_graphs(int n,
    CubicPlanarLabeledEnumAlgorithm algo = CubicPlanarLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CubicPlanarLabeledEnumerationResult result;
    if (n <= 0) return result;
    if (n % 2 != 0) return result;
    if (n < 4) return result;

    detail::CubicPlanarLabeledEnumState root(n);
    detail::cubic_planar_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
