#ifndef GRAPH_RECOGNITION_CATERPILLAR_H
#define GRAPH_RECOGNITION_CATERPILLAR_H

/**
 * @file caterpillar.h
 * @brief Caterpillar graph recognition
 *
 * Verifies the graph is a tree, then checks that removing all leaves yields a path.
 */

#include "util/graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for caterpillar graph recognition
 */
enum class CaterpillarAlgorithm {
    LEAF_REMOVAL /**< leaf removal + path test */
};

/**
 * @brief Result of caterpillar graph recognition
 */
struct CaterpillarResult {
    bool is_caterpillar = false; /**< true if the graph is a caterpillar graph */
};

/**
 * @brief Determines whether a graph is a caterpillar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: LEAF_REMOVAL)
 * @return CaterpillarResult
 *
 * Caterpillar graph iff the graph is a tree and removing all leaves yields a path (or empty).
 */
inline CaterpillarResult check_caterpillar(const Graph& g,
    CaterpillarAlgorithm algo = CaterpillarAlgorithm::LEAF_REMOVAL) {
    (void)algo;
    CaterpillarResult res;

    int n = g.n;

    /* All trees with n <= 2 are caterpillars */
    if (n <= 2) {
        /* n=0: empty graph -> caterpillar */
        /* n=1: isolated vertex -> caterpillar */
        /* n=2: 0 edges (forest on 2 vertices) or 1 edge (path) -> tree only when 1 edge */
        long long m = 0;
        for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
        m /= 2;
        if (n == 0 || m == (long long)n - 1) {
            /* Connectivity check (if n=2 with no edges, disconnected -> not a tree) */
            if (n <= 1) {
                res.is_caterpillar = true;
                return res;
            }
            /* n=2: tree if m=1 */
            if (m == 1) {
                res.is_caterpillar = true;
            }
            return res;
        }
        return res;
    }

    /* Check if it is a tree: m = n - 1 and connected */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n - 1) return res;

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

    /* Remove leaves (degree 1) and compute degrees of remaining vertices */
    std::vector<char> is_leaf(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() == 1) {
            is_leaf[v] = 1;
        }
    }

    /* Among non-leaf vertices, compute the degree within non-leaf neighbors (spine degree) */
    int spine_count = 0;
    for (int v = 1; v <= n; ++v) {
        if (is_leaf[v]) continue;
        ++spine_count;
        int spine_deg = 0;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            if (!is_leaf[g.adj[v][i]]) {
                ++spine_deg;
            }
        }
        /* For a path, each vertex must have degree at most 2 within the spine */
        if (spine_deg > 2) return res;
    }

    /* Spine is empty (all vertices are leaves -> already handled for n <= 2, unreachable here) */
    /* Spine has 1 vertex -> star graph -> caterpillar */
    /* Verify that the spine is a path (connected + each degree <= 2 -> path or cycle) */
    /* Since it is a subgraph of a tree, cycles are impossible -> path */
    res.is_caterpillar = true;
    return res;
}

} // namespace graph_recognition

#endif
