#ifndef GRAPH_RECOGNITION_TRICONNECTED_H
#define GRAPH_RECOGNITION_TRICONNECTED_H

/**
 * @file triconnected.h
 * @brief Triconnected graph recognition
 *
 * Determines whether the graph is triconnected.
 * A triconnected graph is a connected graph with >= 4 vertices such that removing
 * any 2 vertices leaves it connected (vertex connectivity kappa(G) >= 3).
 *
 * Algorithm:
 *   - NAIVE: Removal of all vertex pairs + connectivity check O(n^2 * (n+m))
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for triconnected graph recognition
 */
enum class TriconnectedAlgorithm {
    NAIVE /**< Removal of all vertex pairs (default) */
};

/**
 * @brief Result of triconnected graph recognition
 */
struct TriconnectedResult {
    bool is_triconnected = false; /**< true if the graph is triconnected */
};

/**
 * @brief Determines whether the graph is triconnected
 * @param g Input graph
 * @param algo Algorithm to use (default: NAIVE)
 * @return TriconnectedResult
 *
 * Triconnected graph: >= 4 vertices, connected, remains connected after removal of any 2 vertices.
 * Naive: Checks connectivity of G-{u,v} via BFS for all O(n^2) vertex pairs.
 */
inline TriconnectedResult check_triconnected(const Graph& g,
    TriconnectedAlgorithm algo = TriconnectedAlgorithm::NAIVE) {
    (void)algo;
    TriconnectedResult res;
    res.is_triconnected = false;

    int n = g.n;
    if (n < 4) return res;

    // Necessary condition: all vertices have degree >= 3
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() < 3) return res;
    }

    // Check connectivity of G-{u,v} for all vertex pairs (u, v)
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            // Check if G-{u,v} is connected via BFS
            // Choose starting vertex other than u and v
            int start = -1;
            for (int w = 1; w <= n; ++w) {
                if (w != u && w != v) {
                    start = w;
                    break;
                }
            }
            if (start == -1) return res;  // n-2 == 0, impossible

            std::vector<char> visited(n + 1, 0);
            std::vector<int> queue;
            queue.push_back(start);
            visited[start] = 1;
            int cnt = 1;

            for (size_t qi = 0; qi < queue.size(); ++qi) {
                int w = queue[qi];
                for (size_t i = 0; i < g.adj[w].size(); ++i) {
                    int x = g.adj[w][i];
                    if (x != u && x != v && !visited[x]) {
                        visited[x] = 1;
                        queue.push_back(x);
                        cnt++;
                    }
                }
            }

            if (cnt != n - 2) return res;  // Not connected
        }
    }

    res.is_triconnected = true;
    return res;
}

}  // namespace graph_recognition

#endif
