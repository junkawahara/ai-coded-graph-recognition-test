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
    /**
     * @brief construction_order[i] = the i-th vertex added, for i in [1, n] (size n+1)
     *
     * The first k+1 entries are the initial K_{k+1}; every later vertex is
     * adjacent to exactly k earlier ones, and those k form a clique. This is
     * the simplicial elimination read backwards. Valid only when
     * is_ktree == true.
     */
    std::vector<int> construction_order;
};

namespace detail_ktree {

/**
 * @brief Replays a k-tree construction order against the graph
 *
 * Checks that the first k+1 vertices form a clique and that every later
 * vertex attaches to exactly k earlier ones forming a clique -- the recursive
 * definition, step by step.
 */
inline bool construction_order_is_valid(const Graph& g, int k,
                                        const std::vector<int>& order) {
    int n = g.n;
    if (k < 0 || (int)order.size() != n + 1) return false;
    if (n > 0 && k + 1 > n) return false;

    std::vector<int> pos(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        if (v < 1 || v > n || pos[v] != 0) return false;
        pos[v] = i;
    }

    for (int i = 1; i <= n; ++i) {
        int v = order[i];
        std::vector<int> earlier;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (pos[u] < i) earlier.push_back(u);
        }
        int expected = i <= k + 1 ? i - 1 : k;
        if ((int)earlier.size() != expected) return false;
        for (size_t a = 0; a < earlier.size(); ++a) {
            for (size_t b = a + 1; b < earlier.size(); ++b) {
                if (!g.has_edge(earlier[a], earlier[b])) return false;
            }
        }
    }
    return true;
}

/** @brief Accepts a k-tree result once its construction order replays */
inline void accept(const Graph& g, int k, const std::vector<int>& order, KTreeResult& res) {
    if (!construction_order_is_valid(g, k, order)) return;
    res.construction_order = order;
    res.k = k;
    res.is_ktree = true;
}

/** @brief The identity order 1..n */
inline std::vector<int> identity_order(int n) {
    std::vector<int> order(n + 1, 0);
    for (int v = 1; v <= n; ++v) order[v] = v;
    return order;
}

} // namespace detail_ktree

/**
 * @brief Determines whether the graph is a k-tree
 * @param g Input graph
 * @param algo Algorithm to use (default: SIMPLICIAL_REMOVAL)
 * @return KTreeResult
 *
 * For k >= 1: k-tree iff connected chordal graph where all maximal cliques
 * have size k+1. Equivalently: can reach K_{k+1} by iteratively removing
 * simplicial vertices of degree k. For k = 0 the recursive definition gives
 * exactly the edgeless graphs.
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
        res.construction_order.assign(1, 0);
        return res;
    }

    /* Edgeless graph: 0-tree. The recursive definition (start from K_1,
       attach each new vertex to a 0-clique) yields exactly the edgeless
       graphs, which are disconnected for n >= 2, so this must precede the
       connectivity check. Matches enumerate_ktree_labeled_graphs_reverse_search(n, 0). */
    {
        bool edgeless = true;
        for (int v = 1; v <= n && edgeless; ++v) {
            if (!g.adj[v].empty()) edgeless = false;
        }
        if (edgeless) {
            detail_ktree::accept(g, 0, detail_ktree::identity_order(n), res);
            return res;
        }
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
            detail_ktree::accept(g, n - 1, detail_ktree::identity_order(n), res);
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

    std::vector<int> elimination;
    elimination.reserve(n);
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
        elimination.push_back(v);
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

    /* The surviving K_{k+1} is the start; the eliminated vertices are added
       back in reverse order of removal. */
    std::vector<int> order(n + 1, 0);
    int slot = 0;
    for (int v = 1; v <= n; ++v) {
        if (alive[v]) order[++slot] = v;
    }
    for (size_t i = elimination.size(); i-- > 0;) order[++slot] = elimination[i];

    detail_ktree::accept(g, k_cand, order, res);
    return res;
}

} // namespace graph_recognition

#endif
