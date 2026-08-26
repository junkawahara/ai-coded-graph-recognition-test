#ifndef GRAPH_RECOGNITION_GRAPH_UTILS_H
#define GRAPH_RECOGNITION_GRAPH_UTILS_H

/**
 * @file graph_utils.h
 * @brief Shared elementary graph transformations
 *
 * Complement construction, adjacency matrices and induced subgraphs are needed
 * by many recognizers. Each of them used to carry a private copy; these are the
 * shared definitions all of them now call.
 */

#include "util/graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Builds the complement graph
 * @param g Input graph
 * @return Graph on the same vertex set whose edges are exactly the non-edges of g
 */
inline Graph build_complement(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

/**
 * @brief Builds an adjacency matrix from a graph
 * @param g Input graph
 * @return (n+1) x (n+1) matrix a with a[u][v] = 1 iff uv is an edge (row/column 0 unused)
 */
inline std::vector<std::vector<unsigned char>> build_adj_matrix(const Graph& g) {
    std::vector<std::vector<unsigned char>> a(g.n + 1, std::vector<unsigned char>(g.n + 1, 0));
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            a[u][g.adj[u][i]] = 1;
        }
    }
    return a;
}

/**
 * @brief Builds the complement of an adjacency matrix
 * @param a Adjacency matrix as produced by build_adj_matrix()
 * @return Matrix c with c[u][v] = 1 iff u != v and a[u][v] == 0
 */
inline std::vector<std::vector<unsigned char>> build_complement_matrix(
    const std::vector<std::vector<unsigned char>>& a) {
    int n = (int)a.size() - 1;
    std::vector<std::vector<unsigned char>> c(n + 1, std::vector<unsigned char>(n + 1, 0));
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (a[u][v]) continue;
            c[u][v] = 1;
            c[v][u] = 1;
        }
    }
    return c;
}

/**
 * @brief Builds the subgraph induced on a vertex subset
 * @param g Input graph
 * @param vertices Vertices to keep; the i-th entry becomes vertex i+1 of the result
 * @param orig_of If non-null, filled with the reverse map (size |vertices|+1;
 *                (*orig_of)[i] is the original vertex of result vertex i)
 * @return The induced subgraph, renumbered 1..|vertices|
 *
 * Out-of-range and duplicate entries in `vertices` are dropped, so the result
 * is always a simple graph on at most |vertices| vertices.
 */
inline Graph induced_subgraph(const Graph& g, const std::vector<int>& vertices,
                              std::vector<int>* orig_of = 0) {
    std::vector<int> local(g.n + 1, 0);
    std::vector<int> kept;
    kept.reserve(vertices.size());
    for (size_t i = 0; i < vertices.size(); ++i) {
        int v = vertices[i];
        if (v < 1 || v > g.n) continue;
        if (local[v] != 0) continue;
        kept.push_back(v);
        local[v] = (int)kept.size();
    }

    std::vector<std::pair<int, int>> edges;
    for (size_t i = 0; i < kept.size(); ++i) {
        int u = kept[i];
        for (size_t j = 0; j < g.adj[u].size(); ++j) {
            int w = g.adj[u][j];
            if (local[w] == 0) continue;
            if (local[u] < local[w]) {
                edges.push_back(std::make_pair(local[u], local[w]));
            }
        }
    }

    if (orig_of) {
        orig_of->assign(kept.size() + 1, 0);
        for (size_t i = 0; i < kept.size(); ++i) (*orig_of)[i + 1] = kept[i];
    }
    return Graph((int)kept.size(), edges);
}

} // namespace graph_recognition

#endif
