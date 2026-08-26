#ifndef GRAPH_RECOGNITION_EULERIAN_ENUM_H
#define GRAPH_RECOGNITION_EULERIAN_ENUM_H

/**
 * @file eulerian_labeled_enum.h
 * @brief Enumeration of Eulerian graphs (cycle space basis enumeration)
 *
 * Using the basis of the cycle space (GF(2) vector space) of K_n,
 * enumerates all labeled Eulerian graphs on vertex set {1, ..., n}.
 *
 * Eulerian graphs (all vertices have even degree) form the cycle space
 * of K_n, so each can be uniquely represented as a symmetric difference
 * of a subset of fundamental cycles of a spanning tree. The dimension
 * is d = (n-1)(n-2)/2, and all 2^d labeled Eulerian graphs are enumerated.
 *
 * References:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_labeled_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Eulerian enumeration
 */
enum class EulerianLabeledEnumAlgorithm {
    CYCLE_SPACE_BASIS /**< Cycle space basis enumeration */
};

/**
 * @brief Result of Eulerian enumeration
 */
struct EulerianLabeledEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated Eulerian graphs */
};

namespace detail {

/** @brief Internal state for cycle space basis enumeration */
struct EulerianLabeledEnumState {
    int n;
    int num_edges;   /**< C(n,2): number of edges in K_n */
    int num_cycles;  /**< d = (n-1)(n-2)/2: dimension of the cycle space */
    std::vector<std::pair<int, int>> edge_list;       /**< All edges of K_n (u<v, lexicographic order) */
    std::vector<std::vector<int>> edge_idx;           /**< edge_idx[i][j] = index of edge (i,j) */
    std::vector<std::vector<int>> fundamental_cycles; /**< Edge index list for each fundamental cycle */
    std::vector<char> edge_present;                   /**< Current edge set (XOR state) */
};

/**
 * @brief State initialization: construct edge list and fundamental cycles
 */
inline EulerianLabeledEnumState eulerian_build_state(int n) {
    EulerianLabeledEnumState state;
    state.n = n;

    // Enumerate all edges of K_n in lexicographic order
    state.edge_idx.assign(n + 1, std::vector<int>(n + 1, -1));
    int idx = 0;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.edge_list.push_back(std::make_pair(i, j));
            state.edge_idx[i][j] = idx;
            state.edge_idx[j][i] = idx;
            ++idx;
        }
    }
    state.num_edges = idx;
    state.edge_present.assign(idx, 0);

    // Spanning tree: path 1-2-3-...-n (edges: (k, k+1) for k=1..n-1)
    // Non-tree edges: (i,j) with j > i+1
    // Fundamental cycle of each non-tree edge (i,j): path edges (i,i+1),(i+1,i+2),...,(j-1,j) + edge (i,j)
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 2; j <= n; ++j) {
            std::vector<int> cycle;
            for (int k = i; k < j; ++k) {
                cycle.push_back(state.edge_idx[k][k + 1]);
            }
            cycle.push_back(state.edge_idx[i][j]);
            state.fundamental_cycles.push_back(cycle);
        }
    }
    state.num_cycles = static_cast<int>(state.fundamental_cycles.size());

    return state;
}

/**
 * @brief DFS for cycle space basis enumeration
 *
 * Binary branching on whether to include the cycle_idx-th fundamental cycle,
 * and outputs the edge set once all fundamental cycles have been decided.
 */
inline void eulerian_labeled_enum_dfs(EulerianLabeledEnumState& state, int cycle_idx,
                              std::vector<EnumeratedGraph>* out) {
    if (cycle_idx == state.num_cycles) {
        EnumeratedGraph g;
        g.n = state.n;
        for (int e = 0; e < state.num_edges; ++e) {
            if (state.edge_present[e]) {
                g.edges.push_back(state.edge_list[e]);
            }
        }
        out->push_back(g);
        return;
    }

    // Branch 1: do not include this fundamental cycle
    eulerian_labeled_enum_dfs(state, cycle_idx + 1, out);

    // Branch 2: include this fundamental cycle (XOR toggle)
    const std::vector<int>& cycle = state.fundamental_cycles[cycle_idx];
    for (size_t i = 0; i < cycle.size(); ++i) {
        state.edge_present[cycle[i]] ^= 1;
    }
    eulerian_labeled_enum_dfs(state, cycle_idx + 1, out);
    // Restore
    for (size_t i = 0; i < cycle.size(); ++i) {
        state.edge_present[cycle[i]] ^= 1;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled Eulerian graphs on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only CYCLE_SPACE_BASIS)
 * @return EulerianLabeledEnumerationResult
 *
 * The cycle space of K_n forms a (n-1)(n-2)/2-dimensional vector space
 * over GF(2). Using fundamental cycles of the spanning tree path 1-2-...-n
 * as a basis, enumerates symmetric differences of all 2^d subsets.
 *
 * Each graph is algebraically guaranteed to have all vertices with even degree.
 */
inline EulerianLabeledEnumerationResult
enumerate_eulerian_labeled_graphs(int n,
    EulerianLabeledEnumAlgorithm algo =
        EulerianLabeledEnumAlgorithm::CYCLE_SPACE_BASIS) {
    (void)algo;
    EulerianLabeledEnumerationResult result;
    if (n < 0) return result;
    if (n <= 1) {
        EnumeratedGraph g;
        g.n = n;
        result.graphs.push_back(g);
        return result;
    }
    detail::EulerianLabeledEnumState state = detail::eulerian_build_state(n);
    detail::eulerian_labeled_enum_dfs(state, 0, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
