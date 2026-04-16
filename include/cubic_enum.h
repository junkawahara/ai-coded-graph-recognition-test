#ifndef GRAPH_RECOGNITION_CUBIC_ENUM_H
#define GRAPH_RECOGNITION_CUBIC_ENUM_H

/**
 * @file cubic_enum.h
 * @brief Cubic graph (3-regular graph) enumeration (reverse search)
 *
 * Enumerates all labeled cubic graphs on vertex set {1, ..., n}.
 *
 * Cubic graph: a graph where every vertex has degree exactly 3.
 * No cubic graph exists if n is odd or n < 4.
 *
 * Algorithm:
 *   Vertices are added in order 1, 2, ..., n. When adding vertex x,
 *   neighbors are chosen from vertices in {1,...,x-1} with deg < 3.
 *   Pruning is performed using degree upper bounds and reachability checks.
 *   After all vertices are added, the graph is output if all degrees == 3.
 *
 * References:
 *   Brinkmann, Goedgebeur, McKay, "Generation of Cubic graphs,"
 *   J. Graph Theory 86, 2017
 *   Meringer, "Fast Generation of Regular Graphs and Construction of Cages,"
 *   J. Graph Theory 30, 1999, pp. 137-146
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

enum class CubicEnumAlgorithm {
    REVERSE_SEARCH
};

struct CubicEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

struct CubicEnumState {
    int total_n;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;

    explicit CubicEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

inline void cubic_enum_choose(CubicEnumState& state,
                               const std::vector<int>& available,
                               std::size_t start,
                               int chosen_count,
                               int min_size, int max_size,
                               std::vector<EnumeratedGraph>* out);

inline void cubic_enum_dfs(CubicEnumState& state,
                            std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.deg[v] != 3) return;
        }
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

    cubic_enum_choose(state, available, 0, 0,
                       min_neighbors, max_neighbors, out);
}

inline void cubic_enum_choose(CubicEnumState& state,
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
            cubic_enum_dfs(state, out);
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

        cubic_enum_choose(state, available, i + 1, chosen_count + 1,
                           min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled cubic graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return CubicEnumerationResult
 */
inline CubicEnumerationResult
enumerate_cubic_graphs(int n,
    CubicEnumAlgorithm algo = CubicEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CubicEnumerationResult result;
    if (n <= 0) return result;
    if (n % 2 != 0) return result;
    if (n < 4) return result;

    detail::CubicEnumState root(n);
    detail::cubic_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
