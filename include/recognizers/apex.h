#ifndef GRAPH_RECOGNITION_APEX_H
#define GRAPH_RECOGNITION_APEX_H

/**
 * @file apex.h
 * @brief Apex graph recognition
 *
 * An apex graph is a graph that becomes planar when some vertex is removed.
 * O(n(n+m)) algorithm that removes each vertex v and tests planarity.
 * Minor-closed (hereditary) class, so closed under induced subgraphs.
 */

#include <vector>

#include "util/graph.h"
#include "recognizers/planar.h"

namespace graph_recognition {

/**
 * @brief Algorithm selection for apex recognition
 */
enum class ApexAlgorithm {
    VERTEX_DELETION /**< remove each vertex and test planarity */
};

/**
 * @brief Result of apex recognition
 */
struct ApexResult {
    bool is_apex = false; /**< true if the graph is an apex graph */
};

/**
 * @brief Determines whether a graph is an apex graph
 * @param g Input graph
 * @param algo Algorithm to use (default: VERTEX_DELETION)
 * @return ApexResult
 *
 * Removes each vertex v and checks whether G-v is planar.
 * If any removal yields a planar graph, the graph is apex. All graphs with n <= 5 are apex.
 */
inline ApexResult check_apex(const Graph& g,
    ApexAlgorithm algo = ApexAlgorithm::VERTEX_DELETION) {
    ApexResult res;
    res.is_apex = false;
    (void)algo;

    int n = g.n;
    if (n <= 5) {
        res.is_apex = true;
        return res;
    }

    for (int v = 1; v <= n; ++v) {
        // Build G-v: exclude vertex v and renumber
        std::vector<int> id(n + 1, 0);
        int cnt = 0;
        for (int u = 1; u <= n; ++u) {
            if (u != v) {
                id[u] = ++cnt;
            }
        }

        std::vector<std::pair<int, int> > edges;
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (w == v) continue;
                if (id[w] > id[u]) {
                    edges.push_back(std::make_pair(id[u], id[w]));
                }
            }
        }

        Graph sg(cnt, edges);
        PlanarResult pr = check_planar(sg);
        if (pr.is_planar) {
            res.is_apex = true;
            return res;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
