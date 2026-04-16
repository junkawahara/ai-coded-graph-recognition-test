#ifndef GRAPH_RECOGNITION_FOREST_H
#define GRAPH_RECOGNITION_FOREST_H

/**
 * @file forest.h
 * @brief Forest recognition
 *
 * Determines the absence of cycles by the relationship between edge count and number of connected components.
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for forest recognition
 */
enum class ForestAlgorithm {
    BFS /**< BFS connected component enumeration + edge count check */
};

/**
 * @brief Result of forest recognition
 */
struct ForestResult {
    bool is_forest = false;    /**< true if the graph is a forest */
    int num_components = 0;    /**< Number of connected components */
};

/**
 * @brief Determines whether the graph is a forest
 * @param g Input graph
 * @param algo Algorithm to use (default: BFS)
 * @return ForestResult
 *
 * Forest iff m = n - (number of connected components). Each component is a tree.
 */
inline ForestResult check_forest(const Graph& g,
    ForestAlgorithm algo = ForestAlgorithm::BFS) {
    (void)algo;
    ForestResult res;

    int n = g.n;
    if (n == 0) {
        res.is_forest = true;
        return res;
    }

    /* Count the number of connected components */
    std::vector<char> visited(n + 1, 0);
    int components = 0;
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;
        ++components;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
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
    }

    /* Forest iff m = n - components */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    res.num_components = components;
    if (m == (long long)n - components) {
        res.is_forest = true;
    }
    return res;
}

} // namespace graph_recognition

#endif
