#ifndef GRAPH_RECOGNITION_BICONNECTED_ENUM_H
#define GRAPH_RECOGNITION_BICONNECTED_ENUM_H

/**
 * @file biconnected_labeled_enum.h
 * @brief Biconnected graph enumeration (reverse search)
 *
 * Enumerates all labeled biconnected graphs on vertex set {1, ..., n}
 * using reverse search.
 *
 * parent(G) = remove the vertex with the largest label from G
 *
 * Biconnectedness is not hereditary, so property-based pruning is not
 * performed at intermediate steps. Instead, the following pruning is applied:
 *   1. Endgame connectivity pruning: with at most 1 vertex left to add, prune
 *      as soon as the current graph has 2 or more connected components.
 *      (Counting components earlier is unsound: a single future vertex may
 *      join arbitrarily many components, e.g. K_{2,4}.)
 *   2. Degree lower-bound pruning: at the last 2 levels, prune if some vertex
 *      cannot reach degree 2 even after using all remaining vertices.
 *   3. Full biconnectedness test at the final step
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "recognizers/biconnected.h"
#include "enumerators/chordal_labeled_enum.h"
#include "util/graph.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for biconnected enumeration
 */
enum class BiconnectedLabeledEnumAlgorithm {
    REVERSE_SEARCH /**< reverse search */
};

/**
 * @brief Result of biconnected enumeration
 */
struct BiconnectedLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< array of enumerated biconnected graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct BiconnectedLabeledEnumState {
    int total_n;
    int alive_count;  /**< active vertices are {1, ..., alive_count} */
    std::vector<std::vector<char>> adj;
    std::vector<int> deg;  /**< degree of each vertex */

    explicit BiconnectedLabeledEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief Counts the number of connected components on {1, ..., x}
 */
inline int count_components(const BiconnectedLabeledEnumState& state, int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        // BFS
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
 * @brief DFS for biconnected reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. Since this is not a hereditary class, property-based
 * pruning is not performed; instead, connectivity-based pruning and a full
 * biconnectedness test at the final step are used.
 */
inline void biconnected_labeled_enum_dfs(BiconnectedLabeledEnumState& state,
                                  std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // Final step: full biconnectedness test
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    edges.push_back(std::make_pair(u, v));

        Graph g(state.total_n, edges);
        BiconnectedResult res = check_biconnected(g);
        if (res.is_biconnected) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            graph.edges = edges;
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    // 64+ vertices would overflow 2^k
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // Set edges from x to {1,...,k} based on mask
        int deg_x = 0;
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) {
                state.deg[u]++;
                deg_x++;
            }
        }
        state.deg[x] = deg_x;
        state.alive_count = x;

        bool prune = false;

        // Pruning 1: connectivity
        // A single future vertex may be adjacent to every component (e.g.
        // K_{2,4} has 4 components before its two centers are added), so
        // component count alone only rules out the endgame: with <= 1
        // vertex left, >= 2 components mean the result is disconnected or
        // has the last vertex as a cut vertex.
        int comp = count_components(state, x);
        if (comp > 1 && remaining <= 1) {
            prune = true;
        }

        // Pruning 2: degree lower bound
        // The final graph requires all vertices to have degree >= 2.
        // Vertex v (v <= x) can gain at most remaining edges from remaining vertices.
        // If deg[v] + remaining < 2, it is impossible.
        if (!prune && remaining <= 1) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 2) {
                    prune = true;
                    break;
                }
            }
        }

        if (!prune) {
            biconnected_labeled_enum_dfs(state, out);
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
 * @brief Enumerates all labeled biconnected graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return BiconnectedLabeledEnumerationResult
 *
 * Uses reverse search. Biconnectedness is not hereditary, so intermediate
 * steps only apply endgame pruning (connectivity and minimum-degree checks
 * once at most 1 vertex remains to be added), with a full biconnectedness
 * test at the final step.
 */
inline BiconnectedLabeledEnumerationResult
enumerate_biconnected_labeled_graphs(int n,
    BiconnectedLabeledEnumAlgorithm algo =
        BiconnectedLabeledEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    BiconnectedLabeledEnumerationResult result;
    if (n < 3) return result;
    detail::BiconnectedLabeledEnumState root(n);
    detail::biconnected_labeled_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
