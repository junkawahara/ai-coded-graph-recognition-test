#ifndef GRAPH_RECOGNITION_KTREE_ENUM_H
#define GRAPH_RECOGNITION_KTREE_ENUM_H

/**
 * @file ktree_enum.h
 * @brief Enumeration of k-trees (reverse search)
 *
 * Enumerates all labeled k-trees.
 *
 * A k-tree is defined inductively:
 *   - K_{k+1} is a k-tree
 *   - Adding a new vertex adjacent to a k-clique of a k-tree G yields a k-tree
 *
 * k=1: tree, k=2: maximal outerplanar (n>=3), k=3: Apollonian network.
 * A subclass of chordal graphs with treewidth exactly k.
 *
 * Reverse search parent function: remove the simplicial vertex with the largest label having degree <= k.
 * Child generation: make the new vertex adjacent to a k-clique and check canonicity.
 *
 * References:
 *   Beineke, Pippert, "The number of labeled k-dimensional trees,"
 *   J. Combin. Theory 6(2), 1969
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Result of k-tree enumeration
 */
struct KTreeEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< Array of enumerated k-trees */
};

namespace detail {

/**
 * @brief Determines whether vertex v is simplicial and has degree <= k
 */
inline bool ktree_is_simplicial_leq_k(const ChordalEnumState& state,
                                       int v, int k) {
    std::vector<int> neighbors;
    for (int u = 1; u <= state.total_n; ++u) {
        if (state.alive[u] && state.adj[v][u]) neighbors.push_back(u);
    }
    if ((int)neighbors.size() > k) return false;
    for (std::size_t i = 0; i < neighbors.size(); ++i) {
        for (std::size_t j = i + 1; j < neighbors.size(); ++j) {
            if (!state.adj[neighbors[i]][neighbors[j]]) return false;
        }
    }
    return true;
}

/**
 * @brief Canonical removal vertex for k-tree reverse search
 *
 * Returns the vertex with the largest label that is simplicial and has degree <= k.
 */
inline int ktree_canonical_removed_vertex(const ChordalEnumState& state,
                                           int k) {
    int best = 0;
    for (int v = 1; v <= state.total_n; ++v) {
        if (!state.alive[v]) continue;
        if (ktree_is_simplicial_leq_k(state, v, k)) best = v;
    }
    return best;
}

/**
 * @brief DFS to enumerate cliques of exactly size k
 */
inline void enumerate_exact_k_cliques_dfs(const ChordalEnumState& state,
                                           const std::vector<int>& vertices,
                                           std::size_t start_idx,
                                           int target_size,
                                           std::vector<int>* current,
                                           std::vector<std::vector<int>>* out) {
    if ((int)current->size() == target_size) {
        out->push_back(*current);
        return;
    }
    int remaining_needed = target_size - (int)current->size();
    if ((int)(vertices.size() - start_idx) < remaining_needed) return;

    for (std::size_t i = start_idx; i < vertices.size(); ++i) {
        int v = vertices[i];
        bool ok = true;
        for (std::size_t j = 0; j < current->size(); ++j) {
            if (!state.adj[v][(*current)[j]]) {
                ok = false;
                break;
            }
        }
        if (!ok) continue;
        current->push_back(v);
        enumerate_exact_k_cliques_dfs(state, vertices, i + 1,
                                       target_size, current, out);
        current->pop_back();
    }
}

/**
 * @brief Enumerates all cliques of exactly size k among alive vertices
 */
inline std::vector<std::vector<int>> enumerate_exact_k_cliques(
    const ChordalEnumState& state, int k) {
    std::vector<int> vertices;
    for (int v = 1; v <= state.total_n; ++v) {
        if (state.alive[v]) vertices.push_back(v);
    }
    std::vector<std::vector<int>> cliques;
    if (k == 0) {
        cliques.push_back(std::vector<int>());
        return cliques;
    }
    std::vector<int> current;
    enumerate_exact_k_cliques_dfs(state, vertices, 0, k, &current, &cliques);
    return cliques;
}

/**
 * @brief In-place DFS for k-tree reverse search
 *
 * If alive_count < k: building K_{k+1}. Make new vertex adjacent to all alive vertices.
 * If alive_count >= k: enumerate k-cliques and make new vertex adjacent to each k-clique.
 */
inline void ktree_reverse_search_dfs(ChordalEnumState& state, int k,
                                      std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        EnumeratedGraph graph;
        graph.n = state.total_n;
        graph.edges = collect_edges(state);
        out->push_back(graph);
        return;
    }

    std::vector<int> missing;
    for (int x = 1; x <= state.total_n; ++x) {
        if (!state.alive[x]) missing.push_back(x);
    }

    std::vector<std::vector<int>> cliques;
    if (state.alive_count < k) {
        // Building K_{k+1}: make new vertex adjacent to all alive vertices
        std::vector<int> all_alive;
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.alive[v]) all_alive.push_back(v);
        }
        cliques.push_back(all_alive);
    } else {
        // Enumerate k-cliques
        cliques = enumerate_exact_k_cliques(state, k);
    }

    for (std::size_t i = 0; i < missing.size(); ++i) {
        int x = missing[i];
        for (std::size_t j = 0; j < cliques.size(); ++j) {
            const std::vector<int>& clique = cliques[j];

            // Add x in-place
            state.alive[x] = 1;
            ++state.alive_count;
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 1;
                state.adj[clique[ci]][x] = 1;
            }

            // Canonicity check
            int best = ktree_canonical_removed_vertex(state, k);
            if (best == x) {
                ktree_reverse_search_dfs(state, k, out);
            }

            // Restore
            for (std::size_t ci = 0; ci < clique.size(); ++ci) {
                state.adj[x][clique[ci]] = 0;
                state.adj[clique[ci]][x] = 0;
            }
            state.alive[x] = 0;
            --state.alive_count;
        }
    }
}

}  // namespace detail

/**
 * @brief Enumerates all labeled k-tree on vertex set {1, ..., n}
 * @param n Number of vertices
 * @param k Parameter (treewidth)
 * @return KTreeEnumerationResult
 *
 * Returns empty result if n < k+1 (K_{k+1} is the smallest k-tree).
 * For k=0, returns one empty graph with 0 edges.
 */
inline KTreeEnumerationResult enumerate_ktree_graphs_reverse_search(int n,
                                                                     int k) {
    KTreeEnumerationResult result;
    if (n < 0 || k < 0) return result;
    if (n == 0) {
        /* The empty graph is a 0-tree (consistent with check_ktree) */
        if (k == 0) {
            EnumeratedGraph g;
            g.n = 0;
            result.graphs.push_back(g);
        }
        return result;
    }
    if (n < k + 1) return result;
    detail::ChordalEnumState root(n);
    detail::ktree_reverse_search_dfs(root, k, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
