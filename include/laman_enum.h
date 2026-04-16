#ifndef GRAPH_RECOGNITION_LAMAN_ENUM_H
#define GRAPH_RECOGNITION_LAMAN_ENUM_H

/**
 * @file laman_enum.h
 * @brief Enumeration of Laman graphs (minimally rigid graphs) (reverse search)
 *
 * Enumerates all labeled Laman graphs.
 *
 * Laman graph: n vertices, 2n-3 edges, where every k-vertex subgraph has
 * at most 2k-3 edges ((2,3)-tight). Coincides with minimally rigid graphs in 2D.
 *
 * (2,3)-sparsity is a hereditary property, so after each vertex addition,
 * pruning is performed via subset checking.
 * Tightness (edge count = 2n-3) is checked only at leaves.
 *
 * Pruning:
 *   1. Edge count upper bound: prune immediately if edge_count > 2x-3
 *   2. Edge count reachability: prune if edge_count + max_future < 2n-3
 *   3. Degree lower bound: all vertices need deg >= 2 when n >= 3
 *   4. (2,3)-sparsity: verify all subgraphs via incremental subset checking
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for Laman enumeration
 */
enum class LamanEnumAlgorithm {
    REVERSE_SEARCH /**< Reverse search */
};

/**
 * @brief Result of Laman enumeration
 */
struct LamanEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated Laman graphs */
};

namespace detail {

/** @brief Internal state for reverse search */
struct LamanEnumState {
    int total_n;
    int alive_count;  /**< Alive vertices are {1, ..., alive_count} */
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;  /**< Degree of each vertex */
    int edge_count;        /**< Current number of edges */

    explicit LamanEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0), edge_count(0) {}
};

/**
 * @brief Determines whether the subgraph on {1, ..., x} is (2,3)-sparse (incremental subset check)
 *
 * (2,3)-sparse: every k-vertex subgraph (k >= 2) has at most 2k-3 edges.
 *
 * Incremental optimization: subgraphs on {1,...,x-1} have already been checked,
 * so only subsets containing vertex x are checked (2^(x-1) subsets).
 *
 * @param state Enumeration state
 * @param x Number of vertices to check ({1, ..., x})
 * @return true if (2,3)-sparse
 */
inline bool is_23_sparse(const LamanEnumState& state, int x) {
    if (x <= 2) return true;

    // Only check subsets containing vertex x
    // sub is a bitmask representing a subset of {1,...,x-1} (sub >= 1)
    unsigned int x_minus_1 = static_cast<unsigned int>(x - 1);
    for (unsigned int sub = 1; sub < (1u << x_minus_1); ++sub) {
        // k = |sub| + 1 (including vertex x)
        int k_minus_1 = 0;
        {
            unsigned int tmp = sub;
            while (tmp) { tmp &= (tmp - 1); ++k_minus_1; }
        }
        int k = k_minus_1 + 1;
        int limit = 2 * k - 3;

        // Count edges within the subset
        int edges = 0;
        bool exceeded = false;

        // Edges from vertex x
        for (int i = 0; i < static_cast<int>(x_minus_1) && !exceeded; ++i) {
            if (!(sub & (1u << i))) continue;
            if (state.adj[x][i + 1]) {
                ++edges;
                if (edges > limit) exceeded = true;
            }
        }

        // Edges within {1,...,x-1}
        for (int i = 0; i < static_cast<int>(x_minus_1) && !exceeded; ++i) {
            if (!(sub & (1u << i))) continue;
            for (int j = i + 1; j < static_cast<int>(x_minus_1) && !exceeded;
                 ++j) {
                if (!(sub & (1u << j))) continue;
                if (state.adj[i + 1][j + 1]) {
                    ++edges;
                    if (edges > limit) exceeded = true;
                }
            }
        }

        if (exceeded) return false;
    }
    return true;
}

/**
 * @brief DFS for Laman reverse search
 *
 * Adds vertex alive_count+1 and tries all subsets of {1,...,alive_count}
 * as its neighborhood. (2,3)-sparsity is hereditary, so pruning is performed at each step.
 * Tightness (edge count = 2n-3) is checked only at leaves.
 */
inline void laman_enum_dfs(LamanEnumState& state,
                           std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // Leaf: tightness check
        if (state.edge_count == 2 * state.total_n - 3) {
            EnumeratedGraph graph;
            graph.n = state.total_n;
            for (int u = 1; u <= state.total_n; ++u)
                for (int v = u + 1; v <= state.total_n; ++v)
                    if (state.adj[u][v])
                        graph.edges.push_back(std::make_pair(u, v));
            out->push_back(graph);
        }
        return;
    }

    int x = state.alive_count + 1;
    int k = state.alive_count;
    if (k >= 64) return;
    unsigned long long limit = (k == 0) ? 1ULL : (1ULL << k);

    int remaining = state.total_n - x;  // Number of vertices added after x

    // max_future: maximum number of edges that vertices x+1,...,n can add
    int max_future = 0;
    for (int j = x; j < state.total_n; ++j) max_future += j;

    for (unsigned long long mask = 0; mask < limit; ++mask) {
        // deg_x: degree of vertex x
        int deg_x = 0;
        {
            unsigned long long tmp = mask;
            while (tmp) { tmp &= (tmp - 1); ++deg_x; }
        }
        int new_edge_count = state.edge_count + deg_x;

        // Pruning 1: edge count upper bound (simple sparsity check)
        if (x >= 2 && new_edge_count > 2 * x - 3) continue;

        // Pruning 2: edge count reachability
        if (new_edge_count + max_future < 2 * state.total_n - 3) continue;

        // Set edges
        for (int u = 1; u <= k; ++u) {
            char bit = static_cast<char>((mask >> (u - 1)) & 1);
            state.adj[x][u] = bit;
            state.adj[u][x] = bit;
            if (bit) state.deg[u]++;
        }
        state.deg[x] = deg_x;
        state.edge_count = new_edge_count;
        state.alive_count = x;

        bool prune = false;

        // Pruning 3: degree lower bound (all vertices need deg >= 2 when n >= 3)
        if (state.total_n >= 3) {
            for (int v = 1; v <= x; ++v) {
                if (state.deg[v] + remaining < 2) {
                    prune = true;
                    break;
                }
            }
        }

        // Pruning 4: (2,3)-sparsity (incremental subset check)
        if (!prune && x >= 3) {
            if (!is_23_sparse(state, x)) prune = true;
        }

        if (!prune) {
            laman_enum_dfs(state, out);
        }

        // Restore
        for (int u = 1; u <= k; ++u) {
            if (state.adj[x][u]) state.deg[u]--;
            state.adj[x][u] = 0;
            state.adj[u][x] = 0;
        }
        state.deg[x] = 0;
        state.edge_count -= deg_x;
        state.alive_count = k;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled Laman graphon vertex set {1, ..., n}
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return LamanEnumerationResult
 *
 * Laman graph: a (2,3)-tight graph with n vertices and 2n-3 edges.
 * (2,3)-sparsity is determined via incremental subset checking.
 */
inline LamanEnumerationResult
enumerate_laman_graphs(int n,
    LamanEnumAlgorithm algo = LamanEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    LamanEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        // n=1: output one empty graph (0 edges)
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }
    if (n == 2) {
        // n=2: output one K2 (1 edge)
        EnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }
    detail::LamanEnumState root(n);
    detail::laman_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
