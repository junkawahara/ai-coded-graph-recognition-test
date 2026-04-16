#ifndef GRAPH_RECOGNITION_KREGULAR_ENUM_H
#define GRAPH_RECOGNITION_KREGULAR_ENUM_H

/**
 * @file kregular_enum.h
 * @brief Enumeration of k-regular graphs (reverse search)
 *
 * Enumerates all labeled k-regular graphs on vertex set {1, ..., n}.
 *
 * k-regular graph: a graph where every vertex has degree exactly k.
 * Not a hereditary class; the search space is reduced by pruning with degree constraints.
 *
 * Algorithms:
 *   Vertices are added in order 1, 2, ..., n. When adding vertex x,
 *   neighbors are chosen from vertices in {1,...,x-1} with deg < k.
 *   Pruning is performed using degree upper bounds, reachability, and parity constraints.
 *   After all vertices are added, the graph is output if all degrees == k.
 *
 * References:
 *   Meringer, "Fast Generation of Regular Graphs and Construction of Cages,"
 *   J. Graph Theory 30, 1999, pp. 137-146
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for k-regular enumeration
 */
enum class KRegularEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search (with degree constraints) */
};

/**
 * @brief Result of k-regular enumeration
 */
struct KRegularEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated k-regular graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct KRegularEnumState {
    int total_n;
    int target_k;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;  /**< Current degree of each vertex */

    KRegularEnumState(int n, int k)
        : total_n(n), target_k(k), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief Internal function for subset enumeration + recursion
 *
 * Selects neighbors from available[start..], and tries recursion for
 * subsets of size [min_size, max_size].
 */
inline void kregular_enum_choose(KRegularEnumState& state,
                                  const std::vector<int>& available,
                                  std::size_t start,
                                  int chosen_count,
                                  int min_size, int max_size,
                                  std::vector<EnumeratedGraph>* out);

/**
 * @brief Main DFS for k-regular reverse search
 */
inline void kregular_enum_dfs(KRegularEnumState& state,
                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // All vertices added: check if all degrees == k
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.deg[v] != state.target_k) return;
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
    int k = state.target_k;
    int remaining = state.total_n - x;  // vertices after x

    // Candidates: vertices in {1,...,x-1} with deg < k
    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < k) {
            available.push_back(v);
        }
    }

    // Range of number of neighbors for x
    // x can gain edges from remaining future vertices
    // So chosen neighbors + remaining >= k, i.e. min = max(0, k - remaining)
    // And max = min(k, |available|)
    int min_neighbors = k - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = k;
    if (max_neighbors > (int)available.size()) max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    kregular_enum_choose(state, available, 0, 0,
                          min_neighbors, max_neighbors, out);
}

inline void kregular_enum_choose(KRegularEnumState& state,
                                  const std::vector<int>& available,
                                  std::size_t start,
                                  int chosen_count,
                                  int min_size, int max_size,
                                  std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_n - x;

    if (chosen_count >= min_size) {
        // Try this subset
        state.deg[x] = chosen_count;
        state.alive_count = x;

        // Reachability check: for all vertices v in {1,...,x},
        // deg[v] + (n - x) >= k is required
        bool feasible = true;
        for (int v = 1; v <= x; ++v) {
            if (state.deg[v] + remaining_after_x < state.target_k) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            kregular_enum_dfs(state, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    // Prune by remaining candidate count
    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        // Degree upper bound check for v (already deg < k in choose, but just in case)
        if (state.deg[v] >= state.target_k) continue;

        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;

        kregular_enum_choose(state, available, i + 1, chosen_count + 1,
                              min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled k-regular graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param k Target degree
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return KRegularEnumerationResult
 */
inline KRegularEnumerationResult
enumerate_kregular_graphs_reverse_search(int n, int k,
    KRegularEnumAlgorithm algo =
        KRegularEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    KRegularEnumerationResult result;
    if (n <= 0) {
        if (n == 0 && k == 0) {
            // 0 vertices, 0-regular: one empty graph
            EnumeratedGraph g;
            g.n = 0;
            result.graphs.push_back(g);
        }
        return result;
    }
    // Handshaking lemma: n*k must be even
    if ((long long)n * k % 2 != 0) return result;
    // Impossible if k >= n (maximum degree is n-1)
    if (k >= n) return result;

    detail::KRegularEnumState root(n, k);
    detail::kregular_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
