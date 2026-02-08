#ifndef GRAPH_RECOGNITION_CHORDAL_BIPARTITE_V2_H
#define GRAPH_RECOGNITION_CHORDAL_BIPARTITE_V2_H

#include "bipartite.h"
#include "chordal_bipartite.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

namespace detail_v2 {

// Compute a Doubly Lexicographic Ordering (DLO) of a bipartite adjacency
// matrix by alternating row/column sorts until stable.
// Returns (row_order, col_order).
inline std::pair<std::vector<int>, std::vector<int>>
doubly_lexicographic_ordering(
    const std::vector<int>& x_verts,  // row vertices
    const std::vector<int>& y_verts,  // column vertices
    const Graph& g)
{
    int rx = (int)x_verts.size();
    int ry = (int)y_verts.size();

    // Current orderings (indices into x_verts / y_verts).
    std::vector<int> row_order(rx), col_order(ry);
    for (int i = 0; i < rx; ++i) row_order[i] = i;
    for (int j = 0; j < ry; ++j) col_order[j] = j;

    // Maps from vertex to current position.
    std::vector<int> row_pos(g.n + 1, -1), col_pos(g.n + 1, -1);
    for (int i = 0; i < rx; ++i) row_pos[x_verts[i]] = i;
    for (int j = 0; j < ry; ++j) col_pos[y_verts[j]] = j;

    // Precompute adjacency: for each row vertex, sorted list of column indices.
    // For each column vertex, sorted list of row indices.
    std::vector<std::vector<int>> row_adj(rx), col_adj(ry);
    for (int i = 0; i < rx; ++i) {
        int u = x_verts[i];
        for (size_t t = 0; t < g.adj[u].size(); ++t) {
            int v = g.adj[u][t];
            if (col_pos[v] >= 0) {
                row_adj[i].push_back(col_pos[v]);
            }
        }
    }
    for (int j = 0; j < ry; ++j) {
        int v = y_verts[j];
        for (size_t t = 0; t < g.adj[v].size(); ++t) {
            int u = g.adj[v][t];
            if (row_pos[u] >= 0) {
                col_adj[j].push_back(row_pos[u]);
            }
        }
    }

    // Alternating sort until stable.
    for (int iter = 0; iter < rx + ry + 2; ++iter) {
        bool changed = false;

        // Sort rows by their column pattern (using current col_order positions).
        {
            // For each row, compute the sorted list of column positions.
            std::vector<std::vector<int>> row_key(rx);
            std::vector<int> col_rank(ry);
            for (int j = 0; j < ry; ++j) col_rank[col_order[j]] = j;

            for (int i = 0; i < rx; ++i) {
                int ri = row_order[i];
                row_key[i].assign(ry, 0);
                for (size_t t = 0; t < row_adj[ri].size(); ++t) {
                    row_key[i][col_rank[row_adj[ri][t]]] = 1;
                }
            }

            // Sort row_order stably by row_key.
            std::vector<int> perm(rx);
            for (int i = 0; i < rx; ++i) perm[i] = i;
            std::sort(perm.begin(), perm.end(), [&](int a, int b) {
                return row_key[a] < row_key[b];
            });

            std::vector<int> new_order(rx);
            for (int i = 0; i < rx; ++i) new_order[i] = row_order[perm[i]];
            if (new_order != row_order) {
                changed = true;
                row_order = new_order;
            }
        }

        // Sort columns by their row pattern (using current row_order positions).
        {
            std::vector<std::vector<int>> col_key(ry);
            std::vector<int> row_rank(rx);
            for (int i = 0; i < rx; ++i) row_rank[row_order[i]] = i;

            for (int j = 0; j < ry; ++j) {
                int cj = col_order[j];
                col_key[j].assign(rx, 0);
                for (size_t t = 0; t < col_adj[cj].size(); ++t) {
                    col_key[j][row_rank[col_adj[cj][t]]] = 1;
                }
            }

            std::vector<int> perm(ry);
            for (int j = 0; j < ry; ++j) perm[j] = j;
            std::sort(perm.begin(), perm.end(), [&](int a, int b) {
                return col_key[a] < col_key[b];
            });

            std::vector<int> new_order(ry);
            for (int j = 0; j < ry; ++j) new_order[j] = col_order[perm[j]];
            if (new_order != col_order) {
                changed = true;
                col_order = new_order;
            }
        }

        if (!changed) break;
    }

    // Convert to vertex orderings.
    std::vector<int> x_out(rx), y_out(ry);
    for (int i = 0; i < rx; ++i) x_out[i] = x_verts[row_order[i]];
    for (int j = 0; j < ry; ++j) y_out[j] = y_verts[col_order[j]];
    return std::make_pair(x_out, y_out);
}

// Check if the bi-adjacency matrix (rows = x_order, cols = y_order)
// is Gamma-free.
// Gamma: exists i1 < i2 (rows), j1 < j2 (cols) s.t.
//   M[i1][j1]=1, M[i1][j2]=1, M[i2][j1]=1, M[i2][j2]=0.
inline bool is_gamma_free(
    const std::vector<int>& x_order,
    const std::vector<int>& y_order,
    const Graph& g)
{
    int rx = (int)x_order.size();
    int ry = (int)y_order.size();

    std::vector<int> col_pos(g.n + 1, -1);
    for (int j = 0; j < ry; ++j) col_pos[y_order[j]] = j;

    // For each row, sorted column positions of 1-entries.
    std::vector<std::vector<int>> row_cols(rx);
    for (int i = 0; i < rx; ++i) {
        int u = x_order[i];
        for (size_t t = 0; t < g.adj[u].size(); ++t) {
            int v = g.adj[u][t];
            if (col_pos[v] >= 0) row_cols[i].push_back(col_pos[v]);
        }
        std::sort(row_cols[i].begin(), row_cols[i].end());
    }

    // Build per-row column membership for O(1) lookup.
    std::vector<std::vector<unsigned char>> in_row(rx);
    for (int i = 0; i < rx; ++i) {
        in_row[i].assign(ry, 0);
        for (size_t t = 0; t < row_cols[i].size(); ++t) {
            in_row[i][row_cols[i][t]] = 1;
        }
    }

    for (int i1 = 0; i1 < rx; ++i1) {
        for (int i2 = i1 + 1; i2 < rx; ++i2) {
            bool found_shared = false;
            for (size_t t = 0; t < row_cols[i1].size(); ++t) {
                int j = row_cols[i1][t];
                if (in_row[i2][j]) {
                    found_shared = true;
                } else if (found_shared) {
                    return false;
                }
            }
        }
    }

    return true;
}

} // namespace detail_v2

// Check whether a graph is chordal bipartite (improved version).
// Algorithm:
//   1. Check bipartiteness.
//   2. Compute Doubly Lexicographic Ordering (DLO) of the bipartite
//      adjacency matrix by alternating row/column sorts.
//   3. Check Gamma-free property on both M and M^T.
inline ChordalBipartiteResult check_chordal_bipartite_v2(const Graph& g) {
    ChordalBipartiteResult res;
    res.is_chordal_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;
    if (n <= 2) {
        res.is_chordal_bipartite = true;
        res.color = bip.color;
        return res;
    }

    // Partition vertices by color.
    std::vector<int> x_verts, y_verts;
    for (int v = 1; v <= n; ++v) {
        if (bip.color[v] == 0) x_verts.push_back(v);
        else y_verts.push_back(v);
    }

    // Compute DLO.
    std::pair<std::vector<int>, std::vector<int>> dlo =
        detail_v2::doubly_lexicographic_ordering(x_verts, y_verts, g);

    // Check Gamma-free for M and M^T.
    if (!detail_v2::is_gamma_free(dlo.first, dlo.second, g)) return res;
    if (!detail_v2::is_gamma_free(dlo.second, dlo.first, g)) return res;

    res.is_chordal_bipartite = true;
    res.color = bip.color;
    return res;
}

} // namespace graph_recognition

#endif
