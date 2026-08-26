#ifndef GRAPH_RECOGNITION_TRAPEZOID_H
#define GRAPH_RECOGNITION_TRAPEZOID_H

/**
 * @file trapezoid.h
 * @brief Trapezoid graph recognition
 *
 * Necessary and sufficient conditions for G to be a trapezoid graph:
 *   1. 1. G is a co-comparability graph (complement has transitive orientation)
 *   2. 2. The interval dimension of the corresponding partial order P is at most 2
 *
 * Algorithm: Based on the 2-chain subgraph cover characterization by Cogis (1982).
 *   idim(P) <= 2 <=> the edge set of B(P) can be covered by 2 chain subgraphs
 *              <=> the incompatibility graph I(B) of B(P) is bipartite
 *
 * Construction of B(P) (bipartite graph on (L,R) = (X,X)):
 *   - Edge (x,y) in B <=> x != y and NOT x <_P y
 *
 * Incompatibility condition (2+2 pattern):
 *   Edges (x1,y1) and (x2,y2) are incompatible <=>
 *     x1, y1, x2, y2 are all distinct, x1 <=_P y2 and x2 <=_P y1
 *   (Trivial 2K2 with fewer than 4 distinct elements is excluded as it does not correspond to 2+2)
 */

#include "util/graph.h"
#include "util/graph_utils.h"
#include "decompositions/transitive_orientation.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for trapezoid graph recognition
 */
enum class TrapezoidAlgorithm {
    CHAIN_COVER /**< Interval dimension check via 2-chain subgraph cover */
};

/**
 * @brief Result of trapezoid graph recognition
 */
struct TrapezoidResult {
    bool is_trapezoid = false; /**< true if the graph is a trapezoid graph */
};

namespace detail_trapezoid {

/**
 * @brief Determines whether the interval dimension of partial order P is at most 2
 *
 * Enumerates the edge set of B(P), builds the incompatibility graph I(B),
 * and checks bipartiteness via BFS.
 *
 * @param n Number of vertices
 * @param comp Adjacency matrix of complement graph (1-indexed)
 * @param dir Transitive orientation matrix (dir[u][v]==1 => u <_P v)
 * @return true iff idim(P) ≤ 2
 */
inline bool check_interval_dimension_leq2(
    int n,
    const std::vector<std::vector<unsigned char>>& comp,
    const std::vector<std::vector<int>>& dir) {

    // x <=_P y : x = y or x <_P y
    // x <_P y : comp[x][y] && dir[x][y] == 1

    // Enumerate edges of B(P): (x, y) with x != y, NOT x <_P y
    struct BEdge { int x, y; };
    std::vector<BEdge> edges;

    for (int x = 1; x <= n; ++x) {
        for (int y = 1; y <= n; ++y) {
            if (x == y) continue;
            bool x_lt_y = (comp[x][y] && dir[x][y] == 1);
            if (!x_lt_y) {
                edges.push_back({x, y});
            }
        }
    }

    int m = (int)edges.size();
    if (m == 0) return true;

    // Build incompatibility adjacency lists for I(B), then BFS for bipartiteness.
    // Edges i,j are incompatible (adjacent in I(B)) iff:
    //   x1, y1, x2, y2 are all distinct, x1 <_P y2, x2 <_P y1
    std::vector<std::vector<int>> inc_adj(m);
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            int x1 = edges[i].x, y1 = edges[i].y;
            int x2 = edges[j].x, y2 = edges[j].y;
            if (x1 == x2 || x1 == y2 || y1 == x2 || y1 == y2) continue;
            bool lt1 = (comp[x1][y2] && dir[x1][y2] == 1);
            if (!lt1) continue;
            bool lt2 = (comp[x2][y1] && dir[x2][y1] == 1);
            if (!lt2) continue;
            inc_adj[i].push_back(j);
            inc_adj[j].push_back(i);
        }
    }

    std::vector<int> color(m, -1);
    std::vector<int> bfs;
    bfs.reserve(m);
    for (int i = 0; i < m; ++i) {
        if (color[i] != -1) continue;
        color[i] = 0;
        bfs.clear();
        bfs.push_back(i);
        for (size_t qi = 0; qi < bfs.size(); ++qi) {
            int u = bfs[qi];
            for (size_t k = 0; k < inc_adj[u].size(); ++k) {
                int j = inc_adj[u][k];
                if (color[j] == -1) {
                    color[j] = 1 - color[u];
                    bfs.push_back(j);
                } else if (color[j] == color[u]) {
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace detail_trapezoid

/**
 * @brief Determines whether the graph is a trapezoid graph
 * @param g Input graph
 * @param algo Algorithm to use (default: CHAIN_COVER)
 * @return TrapezoidResult
 */
inline TrapezoidResult check_trapezoid(const Graph& g,
    TrapezoidAlgorithm algo = TrapezoidAlgorithm::CHAIN_COVER) {
    (void)algo;
    TrapezoidResult res;
    res.is_trapezoid = false;

    int n = g.n;
    if (n <= 2) { res.is_trapezoid = true; return res; }

    // Step 1: Co-comparability check (obtain transitive orientation of complement)
    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);

    TransitiveOrientationResult to = transitive_orientation_matrix(c);
    if (!to.is_comparability) return res;

    // Step 2: check if interval dimension <= 2
    if (!detail_trapezoid::check_interval_dimension_leq2(g.n, c, to.dir))
        return res;

    res.is_trapezoid = true;
    return res;
}

} // namespace graph_recognition

#endif
