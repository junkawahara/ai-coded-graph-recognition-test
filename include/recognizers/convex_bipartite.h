#ifndef GRAPH_RECOGNITION_CONVEX_BIPARTITE_H
#define GRAPH_RECOGNITION_CONVEX_BIPARTITE_H

/**
 * @file convex_bipartite.h
 * @brief Convex bipartite graph recognition
 *
 * A bipartite graph G=(X,Y,E) where, when Y vertices are linearly ordered,
 * the neighbors of each x in X form a consecutive interval.
 *
 * Algorithms:
 *   - BRUTE_FORCE: try all permutations of Y side and check C1P (for small graphs)
 *   - C1P: consecutive ones property test using PQ-tree (Booth & Lueker 1976) (default)
 */

#include "recognizers/bipartite.h"
#include "util/graph.h"
#include "decompositions/pq_tree.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for convex bipartite graph recognition
 */
enum class ConvexBipartiteAlgorithm {
    BRUTE_FORCE, /**< C1P check with all permutations */
    C1P          /**< C1P test using PQ-tree (default) */
};

/**
 * @brief Result of convex bipartite graph recognition
 */
struct ConvexBipartiteResult {
    bool is_convex_bipartite = false; /**< true if the graph is convex bipartite */
    std::vector<int> color;   /**< bipartite coloring (valid only when is_convex_bipartite == true) */
    std::vector<int> ordering; /**< vertex ordering for the Y side (valid only when is_convex_bipartite == true) */
};

namespace detail {

/**
 * @brief Checks whether a matrix has the consecutive ones property (C1P) (all permutations)
 *
 * rows[i] represents the set of column indices. Checks whether there exists a permutation
 * of columns such that the 1-positions in each row form a consecutive interval.
 *
 * @param rows Set of column indices with 1 in each row (0-indexed)
 * @param num_cols Number of columns
 * @param out_perm On success, stores the permutation
 * @return true if C1P holds
 */
inline bool check_c1p_brute(
    const std::vector<std::vector<int>>& rows,
    int num_cols,
    std::vector<int>& out_perm) {

    std::vector<int> perm(num_cols);
    for (int i = 0; i < num_cols; ++i) perm[i] = i;

    do {
        std::vector<int> pos(num_cols);
        for (int i = 0; i < num_cols; ++i) pos[perm[i]] = i;

        bool ok = true;
        for (size_t r = 0; r < rows.size() && ok; ++r) {
            if (rows[r].empty()) continue;
            int mn = num_cols, mx = -1;
            for (size_t j = 0; j < rows[r].size(); ++j) {
                int p = pos[rows[r][j]];
                if (p < mn) mn = p;
                if (p > mx) mx = p;
            }
            if (mx - mn + 1 != (int)rows[r].size()) ok = false;
        }
        if (ok) {
            out_perm = perm;
            return true;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    return false;
}

/**
 * @brief Common implementation for convex bipartite graph recognition
 *
 * In a disconnected graph, X/Y roles can be independently swapped per component,
 * so both orientations are tested per component and the one satisfying C1P is selected.
 */
inline ConvexBipartiteResult check_convex_bipartite_impl(
    const Graph& g, bool use_brute) {

    ConvexBipartiteResult res;
    res.is_convex_bipartite = false;

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

    // Classify vertices of each component by bipartite coloring
    std::vector<std::vector<int>> comp_a(num_comps), comp_b(num_comps);
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) comp_a[comp_id[v]].push_back(v);
        else comp_b[comp_id[v]].push_back(v);
    }

    // Determine orientation per component and build Y-side ordering
    std::vector<int> final_color(g.n + 1, -1);
    std::vector<int> y_ordering;

    for (int c = 0; c < num_comps; ++c) {
        std::vector<int>& a = comp_a[c];
        std::vector<int>& b = comp_b[c];

        // Component with no edges: any orientation
        if (a.empty() || b.empty()) {
            for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 0;
            for (size_t i = 0; i < b.size(); ++i) {
                final_color[b[i]] = 1;
                y_ordering.push_back(b[i]);
            }
            continue;
        }

        // Orientation 1: a=X (rows), b=Y (columns) -> Y-side C1P test
        bool found = false;
        {
            int ny = (int)b.size();
            std::vector<int> y_id(g.n + 1, -1);
            for (int i = 0; i < ny; ++i) y_id[b[i]] = i;

            std::vector<std::vector<int>> rows;
            for (size_t i = 0; i < a.size(); ++i) {
                std::vector<int> row;
                for (size_t j = 0; j < g.adj[a[i]].size(); ++j) {
                    int id = y_id[g.adj[a[i]][j]];
                    if (id >= 0) row.push_back(id);
                }
                if (!row.empty()) rows.push_back(row);
            }

            std::vector<int> perm;
            bool ok = use_brute ? check_c1p_brute(rows, ny, perm)
                                : check_c1p_pq_tree(rows, ny, perm);
            if (ok) {
                for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 0;
                for (size_t i = 0; i < b.size(); ++i) final_color[b[i]] = 1;
                for (size_t i = 0; i < perm.size(); ++i)
                    y_ordering.push_back(b[perm[i]]);
                found = true;
            }
        }

        if (found) continue;

        // Orientation 2: b=X (rows), a=Y (columns) -> Y-side C1P test
        {
            int ny = (int)a.size();
            std::vector<int> y_id(g.n + 1, -1);
            for (int i = 0; i < ny; ++i) y_id[a[i]] = i;

            std::vector<std::vector<int>> rows;
            for (size_t i = 0; i < b.size(); ++i) {
                std::vector<int> row;
                for (size_t j = 0; j < g.adj[b[i]].size(); ++j) {
                    int id = y_id[g.adj[b[i]][j]];
                    if (id >= 0) row.push_back(id);
                }
                if (!row.empty()) rows.push_back(row);
            }

            std::vector<int> perm;
            bool ok = use_brute ? check_c1p_brute(rows, ny, perm)
                                : check_c1p_pq_tree(rows, ny, perm);
            if (ok) {
                for (size_t i = 0; i < b.size(); ++i) final_color[b[i]] = 0;
                for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 1;
                for (size_t i = 0; i < perm.size(); ++i)
                    y_ordering.push_back(a[perm[i]]);
                found = true;
            }
        }

        if (!found) return res;
    }

    res.is_convex_bipartite = true;
    res.color = final_color;
    res.ordering = y_ordering;
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is a convex bipartite graph
 * @param g Input graph
 * @param algo Algorithm to use (default: C1P)
 * @return ConvexBipartiteResult
 *
 * A bipartite graph G=(X,Y,E) is convex if, when Y (or X) vertices are linearly ordered,
 * the neighbors of each vertex on the opposite side form a consecutive interval.
 * Uses PQ-tree (Booth & Lueker 1976) to test the consecutive ones property.
 */
inline ConvexBipartiteResult check_convex_bipartite(const Graph& g,
    ConvexBipartiteAlgorithm algo = ConvexBipartiteAlgorithm::C1P) {
    switch (algo) {
        case ConvexBipartiteAlgorithm::BRUTE_FORCE:
            return detail::check_convex_bipartite_impl(g, true);
        case ConvexBipartiteAlgorithm::C1P:
            return detail::check_convex_bipartite_impl(g, false);
        default:
            break;
    }
    return ConvexBipartiteResult();
}

} // namespace graph_recognition

#endif
