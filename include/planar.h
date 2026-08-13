#ifndef GRAPH_RECOGNITION_PLANAR_H
#define GRAPH_RECOGNITION_PLANAR_H

/**
 * @file planar.h
 * @brief Planar graph recognition
 *
 * Two algorithms are available:
 *   - LEFT_RIGHT (default): the linear-time left-right planarity criterion
 *     (de Fraysseix, Ossona de Mendez, Rosenstiehl; see planarity_lr.h).
 *   - MINOR_CHECK: Kuratowski's theorem via explicit K5/K3,3 minor search.
 *     Exact but exponential in the worst case, and only practical for tiny
 *     graphs; kept as a cross-check for the default algorithm.
 * Both share a fast filter using the edge count upper bound m <= 3n-6.
 */

#include "graph.h"
#include "minor.h"
#include "planarity_lr.h"

#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for planar graph recognition
 */
enum class PlanarAlgorithm {
    LEFT_RIGHT,  /**< Left-right planarity criterion, O(n + m) */
    MINOR_CHECK  /**< K5/K3,3 minor check, exponential worst case */
};

/**
 * @brief Result of planar graph recognition
 */
struct PlanarResult {
    bool is_planar = false; /**< true if the graph is a planar graph */
};

namespace detail_planar {

/** @brief Planarity via explicit K5/K3,3 minor search (exponential) */
inline bool is_planar_minor(const Graph& g) {
    int n = g.n;
    if (n <= 4) return true;

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    // Edge count upper bound for simple planar graphs
    if (n >= 3 && m > 3LL * n - 6) return false;

    // Check K5/K3,3 minor for each connected component
    std::vector<bool> visited(n + 1, false);
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;

        // Collect connected component via BFS
        std::vector<int> comp;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            comp.push_back(v);
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int w = g.adj[v][i];
                if (!visited[w]) {
                    visited[w] = true;
                    q.push(w);
                }
            }
        }

        int cn = static_cast<int>(comp.size());
        if (cn <= 4) continue;

        // Edge count check for component
        long long cm = 0;
        for (size_t i = 0; i < comp.size(); ++i)
            cm += (long long)g.adj[comp[i]].size();
        cm /= 2;
        if (cm > 3LL * cn - 6) return false;

        // Build subgraph of component and check for minor
        // Renumber vertices to 1..cn
        std::vector<int> id(n + 1, 0);
        for (int i = 0; i < cn; ++i) id[comp[i]] = i + 1;

        std::vector<std::pair<int, int> > edges;
        for (int i = 0; i < cn; ++i) {
            int u = comp[i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int v = g.adj[u][j];
                if (id[v] > id[u]) {
                    edges.push_back(std::make_pair(id[u], id[v]));
                }
            }
        }

        Graph sg(cn, edges);
        detail_minor::MinorState st = detail_minor::build_minor_state(sg);

        detail_minor::MinorChecker k5(detail_minor::MinorTarget::K5);
        if (k5.has_minor(st)) return false;

        detail_minor::MinorChecker k33(detail_minor::MinorTarget::K33);
        if (k33.has_minor(st)) return false;
    }

    return true;
}

} // namespace detail_planar

/**
 * @brief Determines whether the graph is a planar graph
 * @param g Input graph
 * @param algo Algorithm to use (default: LEFT_RIGHT)
 * @return PlanarResult
 */
inline PlanarResult check_planar(const Graph& g,
    PlanarAlgorithm algo = PlanarAlgorithm::LEFT_RIGHT) {
    PlanarResult res;
    if (algo == PlanarAlgorithm::MINOR_CHECK) {
        res.is_planar = detail_planar::is_planar_minor(g);
    } else {
        res.is_planar = detail_planar_lr::is_planar_lr(g);
    }
    return res;
}

} // namespace graph_recognition

#endif
