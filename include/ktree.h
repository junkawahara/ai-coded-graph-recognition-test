#ifndef GRAPH_RECOGNITION_KTREE_H
#define GRAPH_RECOGNITION_KTREE_H

/**
 * @file ktree.h
 * @brief k-tree recognition
 *
 * A k-tree is a type of chordal graph, starting from K_{k+1} and at each step
 * adding a new vertex adjacent to a k-clique.
 * Recognition by iteratively removing simplicial vertices of minimum degree k.
 */

#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for k-tree recognition
 */
enum class KTreeAlgorithm {
    SIMPLICIAL_REMOVAL /**< Simplicial vertex removal */
};

/**
 * @brief Result of k-tree recognition
 */
struct KTreeResult {
    bool is_ktree = false; /**< true if the graph is a k-tree */
    int k = -1;            /**< Value of k */
};

/**
 * @brief Determines whether the graph is a k-tree
 * @param g Input graph
 * @param algo Algorithm to use (default: SIMPLICIAL_REMOVAL)
 * @return KTreeResult
 *
 * k-tree iff connected chordal graph where all maximal cliques have size k+1.
 * Equivalently: can reach K_{k+1} by iteratively removing simplicial vertices of degree k.
 */
inline KTreeResult check_ktree(const Graph& g,
    KTreeAlgorithm algo = KTreeAlgorithm::SIMPLICIAL_REMOVAL) {
    (void)algo;
    KTreeResult res;

    int n = g.n;

    /* Empty graph: 0-tree */
    if (n == 0) {
        res.is_ktree = true;
        res.k = 0;
        return res;
    }

    /* Connectivity check */
    std::vector<char> visited(n + 1, 0);
    std::vector<int> queue;
    queue.push_back(1);
    visited[1] = 1;
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (!visited[u]) {
                visited[u] = 1;
                queue.push_back(u);
            }
        }
    }
    if ((int)queue.size() != n) return res;

    /* Edge count computation */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* Use minimum degree as candidate for k */
    int k_cand = n;
    for (int v = 1; v <= n; ++v) {
        int d = (int)g.adj[v].size();
        if (d < k_cand) k_cand = d;
    }

    /* If n <= k_cand: complete graph is a (n-1)-tree */
    if (n <= k_cand + 1) {
        if (m == (long long)n * (n - 1) / 2) {
            res.is_ktree = true;
            res.k = n - 1;
        }
        return res;
    }

    /* Edge count check: m = k*n - k*(k+1)/2 */
    long long expected_m = (long long)k_cand * n - (long long)k_cand * (k_cand + 1) / 2;
    if (m != expected_m) return res;

    /* Adjacency matrix (dynamic) */
    std::vector<std::vector<char>> adj_mat(n + 1, std::vector<char>(n + 1, 0));
    std::vector<int> deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        deg[v] = (int)g.adj[v].size();
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            adj_mat[v][g.adj[v][i]] = 1;
        }
    }

    std::vector<char> alive(n + 1, 1);

    /* Queue for collecting vertices of degree k_cand */
    std::vector<int> cand;
    for (int v = 1; v <= n; ++v) {
        if (deg[v] == k_cand) cand.push_back(v);
    }

    int removed = 0;

    int target_remain = k_cand + 1;

    while (!cand.empty() && removed < n - target_remain) {
        int v = cand.back();
        cand.pop_back();
        if (!alive[v] || deg[v] != k_cand) continue;

        /* Check if N(v) is a clique */
        std::vector<int> nbrs;
        for (int u = 1; u <= n; ++u) {
            if (alive[u] && adj_mat[v][u]) nbrs.push_back(u);
        }
        if ((int)nbrs.size() != k_cand) continue;

        bool is_clique = true;
        for (size_t i = 0; i < nbrs.size() && is_clique; ++i) {
            for (size_t j = i + 1; j < nbrs.size() && is_clique; ++j) {
                if (!adj_mat[nbrs[i]][nbrs[j]]) is_clique = false;
            }
        }
        if (!is_clique) return res;

        /* Remove v */
        alive[v] = 0;
        for (size_t i = 0; i < nbrs.size(); ++i) {
            adj_mat[v][nbrs[i]] = 0;
            adj_mat[nbrs[i]][v] = 0;
            --deg[nbrs[i]];
            if (deg[nbrs[i]] == k_cand) cand.push_back(nbrs[i]);
        }
        deg[v] = 0;
        ++removed;
    }

    /* Remaining vertices must form K_{k+1} */
    if (removed != n - (k_cand + 1)) return res;

    int remain_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (alive[v]) {
            if (deg[v] != k_cand) return res;
            ++remain_count;
        }
    }
    if (remain_count != k_cand + 1) return res;

    res.is_ktree = true;
    res.k = k_cand;
    return res;
}

} // namespace graph_recognition

#endif
