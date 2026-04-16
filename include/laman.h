#ifndef GRAPH_RECOGNITION_LAMAN_H
#define GRAPH_RECOGNITION_LAMAN_H

/**
 * @file laman.h
 * @brief Laman graph recognition
 *
 * A Laman graph is a minimally rigid graph in 2 dimensions,
 * (2,3)-tight: m = 2n - 3 and for every subset S (|S| >= 2),
 * edges(S) <= 2|S| - 3.
 *
 * Algorithms: pebble game O(n^2)
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for Laman graph recognition
 */
enum class LamanAlgorithm {
    PEBBLE_GAME /**< Pebble game O(n^2) */
};

/**
 * @brief Result of Laman graph recognition
 */
struct LamanResult {
    bool is_laman = false; /**< true if the graph is a Laman graph */
};

namespace detail {

/**
 * @brief Pebble game based (2,3)-sparsity check
 *
 * Places 2 pebbles on each vertex, and when adding each edge,
 * checks whether 3 pebbles can be secured from the reachable vertices of the endpoints.
 */
inline bool laman_pebble_game(const Graph& g) {
    int n = g.n;
    std::vector<int> pebbles(n + 1, 2);
    /* out[v] = targets of directed edges from v */
    std::vector<std::vector<int>> out(n + 1);

    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; /* Process each edge only once */

            /* Search until 3 pebbles are reachable from u, v */
            int need = 3 - pebbles[u] - pebbles[v];
            for (int attempts = 0; attempts < need; ++attempts) {
                /* BFS to find a vertex with pebbles from u or v */
                bool found = false;
                for (int start_idx = 0; start_idx < 2 && !found; ++start_idx) {
                    int start = (start_idx == 0) ? u : v;
                    std::vector<int> bfs_queue;
                    std::vector<int> parent(n + 1, -1);
                    parent[start] = start;
                    bfs_queue.push_back(start);

                    for (size_t qi = 0; qi < bfs_queue.size() && !found; ++qi) {
                        int w = bfs_queue[qi];
                        if (w != u && w != v && pebbles[w] > 0) {
                            /* Move pebble to start: reverse the path */
                            int cur = w;
                            pebbles[w]--;
                            while (cur != start) {
                                int p = parent[cur];
                                /* Reverse edge p->cur to cur->p */
                                /* Remove cur from out[p] and add p to out[cur] */
                                std::vector<int>& po = out[p];
                                for (size_t k = 0; k < po.size(); ++k) {
                                    if (po[k] == cur) {
                                        po[k] = po.back();
                                        po.pop_back();
                                        break;
                                    }
                                }
                                out[cur].push_back(p);
                                cur = p;
                            }
                            pebbles[start]++;
                            found = true;
                        }
                        if (!found) {
                            for (size_t j = 0; j < out[w].size(); ++j) {
                                int x = out[w][j];
                                if (parent[x] == -1) {
                                    parent[x] = w;
                                    bfs_queue.push_back(x);
                                }
                            }
                        }
                    }
                }
                if (!found) return false; /* sparsity violation */
            }

            /* Orient edge (u, v) */
            if (pebbles[u] > 0) {
                out[u].push_back(v);
                pebbles[u]--;
            } else {
                out[v].push_back(u);
                pebbles[v]--;
            }
        }
    }
    return true;
}

} // namespace detail

/**
 * @brief Determines whether the graph is a Laman graph
 * @param g Input graph
 * @param algo Algorithm to use (default: PEBBLE_GAME)
 * @return LamanResult
 *
 * Laman graph iff (2,3)-tight: m = 2n - 3 and (2,3)-sparse.
 */
inline LamanResult check_laman(const Graph& g,
    LamanAlgorithm algo = LamanAlgorithm::PEBBLE_GAME) {
    (void)algo;
    LamanResult res;

    int n = g.n;
    if (n <= 1) return res; /* n=0: no edges, n=1: m=0 != 2*1-3=-1 */

    /* Edge count check: m = 2n - 3 */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != 2LL * n - 3) return res;

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

    /* Pebble game (2,3)-sparsity check */
    if (!detail::laman_pebble_game(g)) return res;

    res.is_laman = true;
    return res;
}

} // namespace graph_recognition

#endif
