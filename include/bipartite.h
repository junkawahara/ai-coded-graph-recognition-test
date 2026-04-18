#ifndef GRAPH_RECOGNITION_BIPARTITE_H
#define GRAPH_RECOGNITION_BIPARTITE_H

/**
 * @file bipartite.h
 * @brief Bipartite graph recognition
 *
 * Determines bipartiteness using 2-coloring by BFS.
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for bipartite graph recognition
 */
enum class BipartiteAlgorithm {
    BFS /**< 2-coloring by BFS */
};

/**
 * @brief Result of bipartite graph recognition
 */
struct BipartiteResult {
    bool is_bipartite = false;          /**< true if the graph is bipartite */
    std::vector<int> color;     /**< color[v]: color of vertex v (0 or 1). Valid only when is_bipartite == true */
};

/**
 * @brief Determines whether a graph is bipartite
 * @param g Input graph
 * @param algo Algorithm selector (currently only BFS is implemented)
 * @return BipartiteResult
 *
 * Attempts 2-coloring by BFS; if no conflict arises, the graph is bipartite.
 */
inline BipartiteResult check_bipartite(const Graph& g,
    BipartiteAlgorithm algo = BipartiteAlgorithm::BFS) {
    (void)algo;
    BipartiteResult res;
    res.is_bipartite = true;
    res.color.assign(g.n + 1, -1);

    std::queue<int> q;
    for (int s = 1; s <= g.n; ++s) {
        if (res.color[s] != -1) continue;
        res.color[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (res.color[u] == -1) {
                    res.color[u] = 1 - res.color[v];
                    q.push(u);
                    continue;
                }
                if (res.color[u] == res.color[v]) {
                    res.is_bipartite = false;
                    return res;
                }
            }
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
