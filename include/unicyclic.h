#ifndef GRAPH_RECOGNITION_UNICYCLIC_H
#define GRAPH_RECOGNITION_UNICYCLIC_H

/**
 * @file unicyclic.h
 * @brief Unicyclic graph recognition
 *
 * Determined by connectivity and edge count = vertex count.
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for unicyclic graph recognition
 */
enum class UnicyclicAlgorithm {
    BFS /**< Connectivity via BFS + edge count check */
};

/**
 * @brief Result of unicyclic graph recognition
 */
struct UnicyclicResult {
    bool is_unicyclic = false; /**< true if the graph is unicyclic */
};

/**
 * @brief Determines whether the graph is unicyclic
 * @param g Input graph
 * @param algo Algorithm to use (default: BFS)
 * @return UnicyclicResult
 *
 * Unicyclic graph <=> connected and m = n.
 */
inline UnicyclicResult check_unicyclic(const Graph& g,
    UnicyclicAlgorithm algo = UnicyclicAlgorithm::BFS) {
    (void)algo;
    UnicyclicResult res;

    int n = g.n;
    if (n == 0) return res;

    /* Edge count check */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n) return res;

    /* Connectivity check via BFS */
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

    res.is_unicyclic = true;
    return res;
}

} // namespace graph_recognition

#endif
