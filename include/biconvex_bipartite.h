#ifndef GRAPH_RECOGNITION_BICONVEX_BIPARTITE_H
#define GRAPH_RECOGNITION_BICONVEX_BIPARTITE_H

/**
 * @file biconvex_bipartite.h
 * @brief Biconvex bipartite graph recognition
 *
 * A bipartite graph G=(X,Y,E) where, when linear orderings are given to both
 * the X side and Y side, the neighbors on the opposite side of each vertex form
 * a consecutive interval. In other words, a bipartite graph whose bipartite
 * adjacency matrix satisfies the consecutive ones property (C1P) for both rows
 * and columns.
 *
 * Algorithms:
 *   - BRUTE_FORCE: try all permutations on both sides and check C1P (for small graphs)
 *   - C1P: C1P test using PQ-tree (Booth & Lueker 1976) (default)
 *
 * References:
 *   - Abbas & Stewart, "Biconvex graphs: ordering and algorithms"
 *   - Yu & Chen
 */

#include "bipartite.h"
#include "convex_bipartite.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for biconvex bipartite graph recognition
 */
enum class BiconvexBipartiteAlgorithm {
    BRUTE_FORCE, /**< check C1P with all permutations on both sides */
    C1P          /**< C1P test using PQ-tree (default) */
};

/**
 * @brief Result of biconvex bipartite graph recognition
 */
struct BiconvexBipartiteResult {
    bool is_biconvex_bipartite = false; /**< true if the graph is biconvex bipartite */
    std::vector<int> color;      /**< bipartite coloring (valid only when true) */
    std::vector<int> x_ordering; /**< vertex ordering for the X side (valid only when true) */
    std::vector<int> y_ordering; /**< vertex ordering for the Y side (valid only when true) */
};

namespace detail {

/**
 * @brief Helper to check C1P for one side of the bipartite adjacency matrix
 *
 * Takes each vertex in row_verts as a row and each vertex in col_verts as a column,
 * and finds a column permutation such that the adjacent columns in each row form a consecutive interval.
 *
 * @param g Input graph
 * @param row_verts Vertex set corresponding to rows
 * @param col_verts Vertex set corresponding to columns
 * @param out_col_perm On success, stores the column permutation (index sequence of col_verts)
 * @param use_brute If true, brute-force all permutations; if false, use partition refinement
 * @return true if C1P holds
 */
inline bool check_one_side_c1p(
    const Graph& g,
    const std::vector<int>& row_verts,
    const std::vector<int>& col_verts,
    std::vector<int>& out_col_perm,
    bool use_brute) {

    int num_cols = (int)col_verts.size();
    if (num_cols == 0) {
        out_col_perm.clear();
        return true;
    }

    std::vector<int> col_id(g.n + 1, -1);
    for (int i = 0; i < num_cols; ++i) col_id[col_verts[i]] = i;

    std::vector<std::vector<int>> rows;
    for (size_t i = 0; i < row_verts.size(); ++i) {
        int v = row_verts[i];
        std::vector<int> row;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int id = col_id[g.adj[v][j]];
            if (id >= 0) row.push_back(id);
        }
        if (!row.empty()) rows.push_back(row);
    }

    std::vector<int> perm;
    bool ok;
    if (use_brute) {
        ok = check_c1p_brute(rows, num_cols, perm);
    } else {
        ok = check_c1p_pq_tree(rows, num_cols, perm);
    }

    if (ok) {
        out_col_perm = perm;
    }
    return ok;
}

/**
 * @brief Common implementation for biconvex bipartite graph recognition
 *
 * In a disconnected bipartite graph, each connected component can independently
 * assign X/Y roles, so the Y-side C1P orientation is determined per component and combined.
 */
inline BiconvexBipartiteResult check_biconvex_bipartite_impl(
    const Graph& g, bool use_brute) {

    BiconvexBipartiteResult res;
    res.is_biconvex_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    // Find connected components via BFS
    std::vector<int> comp_id(g.n + 1, -1);
    int num_comps = 0;
    for (int s = 1; s <= g.n; ++s) {
        if (comp_id[s] != -1) continue;
        int c = num_comps++;
        comp_id[s] = c;
        std::queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (comp_id[u] == -1) {
                    comp_id[u] = c;
                    q.push(u);
                }
            }
        }
    }

    // Classify vertices of each component by bipartition color
    std::vector<std::vector<int>> comp_a(num_comps), comp_b(num_comps);
    for (int v = 1; v <= g.n; ++v) {
        int c = comp_id[v];
        if (bip.color[v] == 0) comp_a[c].push_back(v);
        else comp_b[c].push_back(v);
    }

    // Check C1P on both sides for each component
    // Biconvex requires C1P(M) AND C1P(M^T), and this condition is orientation-independent
    std::vector<int> x_verts, y_verts;
    for (int c = 0; c < num_comps; ++c) {
        std::vector<int>& a = comp_a[c];
        std::vector<int>& b = comp_b[c];

        // Component with no edges: either orientation is fine
        if (a.empty() || b.empty()) {
            for (size_t i = 0; i < a.size(); ++i) x_verts.push_back(a[i]);
            for (size_t i = 0; i < b.size(); ++i) y_verts.push_back(b[i]);
            continue;
        }

        // Biconvex requires C1P on both sides: check(a->b) AND check(b->a)
        std::vector<int> dummy_perm;
        bool ab_ok = check_one_side_c1p(g, a, b, dummy_perm, use_brute);
        bool ba_ok = check_one_side_c1p(g, b, a, dummy_perm, use_brute);
        if (!ab_ok || !ba_ok) {
            return res;
        }

        for (size_t i = 0; i < a.size(); ++i) x_verts.push_back(a[i]);
        for (size_t i = 0; i < b.size(); ++i) y_verts.push_back(b[i]);
    }

    if (x_verts.empty() || y_verts.empty()) {
        res.is_biconvex_bipartite = true;
        res.color.assign(g.n + 1, -1);
        for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 0;
        for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 1;
        res.x_ordering = x_verts;
        res.y_ordering = y_verts;
        return res;
    }

    // Overall Y-side C1P test (after orientation determined per component)
    std::vector<int> y_perm;
    if (!check_one_side_c1p(g, x_verts, y_verts, y_perm, use_brute)) {
        return res;
    }

    // X-side C1P test (rows=Y, columns=X)
    std::vector<int> x_perm;
    if (!check_one_side_c1p(g, y_verts, x_verts, x_perm, use_brute)) {
        return res;
    }

    // Both sides satisfy C1P -> biconvex bipartite graph
    res.is_biconvex_bipartite = true;
    res.color.assign(g.n + 1, -1);
    for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 0;
    for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 1;

    // Convert permutation to vertices
    res.x_ordering.resize(x_verts.size());
    for (size_t i = 0; i < x_perm.size(); ++i) {
        res.x_ordering[i] = x_verts[x_perm[i]];
    }
    res.y_ordering.resize(y_verts.size());
    for (size_t i = 0; i < y_perm.size(); ++i) {
        res.y_ordering[i] = y_verts[y_perm[i]];
    }

    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a biconvex bipartite graph
 * @param g Input graph
 * @param algo Algorithm to use (default: C1P)
 * @return BiconvexBipartiteResult
 *
 * A bipartite graph G=(X,Y,E) is biconvex if, when linear orderings are
 * given to both X and Y sides, the neighbors on the opposite side of each
 * vertex form a consecutive interval. That is, both rows and columns of
 * the bipartite adjacency matrix satisfy C1P.
 */
inline BiconvexBipartiteResult check_biconvex_bipartite(const Graph& g,
    BiconvexBipartiteAlgorithm algo = BiconvexBipartiteAlgorithm::C1P) {
    switch (algo) {
        case BiconvexBipartiteAlgorithm::BRUTE_FORCE:
            return detail::check_biconvex_bipartite_impl(g, true);
        case BiconvexBipartiteAlgorithm::C1P:
            return detail::check_biconvex_bipartite_impl(g, false);
        default:
            break;
    }
    return BiconvexBipartiteResult();
}

} // namespace graph_recognition

#endif
