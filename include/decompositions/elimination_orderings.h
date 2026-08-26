#ifndef GRAPH_RECOGNITION_ELIMINATION_ORDERINGS_H
#define GRAPH_RECOGNITION_ELIMINATION_ORDERINGS_H

/**
 * @file elimination_orderings.h
 * @brief Strong elimination orderings of strongly chordal graphs
 *
 * A vertex is *simple* when the closed neighbourhoods of its neighbours are
 * linearly ordered by inclusion, and a graph is strongly chordal exactly when
 * it can be reduced to nothing by repeatedly deleting simple vertices. That
 * makes any simple-vertex elimination enough to recognize the class -- but not
 * enough to be a strong elimination ordering, which is a strictly stronger
 * requirement.
 *
 * The ordering here therefore follows Farber's construction: at each step the
 * strict closed-neighbourhood inclusions of the remaining graph are added to a
 * partial order, and the simple vertex removed is one that is minimal in it
 * (smallest label among ties). The result is checked for the defining property
 * before being returned.
 *
 * An ordering v1, ..., vn is a strong elimination ordering iff for all i < j
 * and k < l, vk and vl in N[vi] and vk in N[vj] imply vl in N[vj] -- that is,
 * iff the closed neighbourhood matrix in this order contains no
 *
 *   1 1
 *   1 0
 *
 * pattern. Perfect elimination follows from it: taking the columns to be j
 * and k reduces to the simplicial condition.
 *
 * References:
 *   - M. Farber, "Characterizations of strongly chordal graphs", Discrete
 *     Mathematics 43 (1983) 173-189.
 */

#include "util/graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief A strong elimination ordering
 */
struct StrongEliminationResult {
    bool success = false;    /**< true if the graph is strongly chordal */
    std::vector<int> order;  /**< order[i] = the i-th vertex eliminated, for i in [1, n] (size n+1) */
    std::vector<int> number; /**< number[v] = position of v in order, in [1, n] (size n+1) */
};

namespace detail_elimination {

/** @brief Collects the neighbours of v that are still alive */
inline void collect_alive_neighbors(const Graph& g, int v,
                                    const std::vector<unsigned char>& alive,
                                    std::vector<int>* neighbors) {
    neighbors->clear();
    for (size_t i = 0; i < g.adj[v].size(); ++i) {
        int u = g.adj[v][i];
        if (alive[u]) neighbors->push_back(u);
    }
}

/** @brief Whether N[x] is contained in N[y] within the alive subgraph */
inline bool is_closed_neighborhood_subset(const Graph& g, int x, int y,
                                          const std::vector<unsigned char>& alive) {
    // x lies in N[x], so it must lie in N[y] too.
    if (x != y && !g.has_edge(x, y)) return false;

    for (size_t i = 0; i < g.adj[x].size(); ++i) {
        int z = g.adj[x][i];
        if (!alive[z]) continue;
        if (z == y) continue;
        if (!g.has_edge(y, z)) return false;
    }
    return true;
}

/** @brief Whether the inclusion is strict, using alive degrees to rule out equality */
inline bool is_strict_closed_neighborhood_subset(const Graph& g, int x, int y,
                                                 const std::vector<unsigned char>& alive,
                                                 const std::vector<int>& alive_degree) {
    if (alive_degree[x] >= alive_degree[y]) return false;
    return is_closed_neighborhood_subset(g, x, y, alive);
}

/**
 * @brief Whether v is a simple vertex of the alive subgraph
 *
 * v must be simplicial, and the closed neighbourhoods of its neighbours must
 * be pairwise comparable by inclusion.
 */
inline bool is_simple_vertex(const Graph& g, int v,
                             const std::vector<unsigned char>& alive,
                             std::vector<int>* neighbors) {
    collect_alive_neighbors(g, v, alive, neighbors);

    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (!g.has_edge(x, y)) return false;
        }
    }

    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (is_closed_neighborhood_subset(g, x, y, alive)) continue;
            if (is_closed_neighborhood_subset(g, y, x, alive)) continue;
            return false;
        }
    }
    return true;
}

/**
 * @brief Whether an ordering has the strong elimination property
 *
 * Tests the pattern directly, but a pair of rows at a time: with A the columns
 * in both closed neighbourhoods and B the columns only in the first, the
 * pattern exists exactly when min(A) < max(B).
 */
inline bool is_strong_elimination_order(const Graph& g, const std::vector<int>& number) {
    int n = g.n;
    if ((int)number.size() != n + 1) return false;

    int words = (n + 64) / 64;
    std::vector<unsigned long long> row((size_t)(n + 1) * words, 0ULL);
    for (int v = 1; v <= n; ++v) {
        int i = number[v];
        if (i < 1 || i > n) return false;
        // The closed neighbourhood of v, as a set of positions.
        row[(size_t)i * words + (i >> 6)] |= 1ULL << (i & 63);
        for (size_t t = 0; t < g.adj[v].size(); ++t) {
            int p = number[g.adj[v][t]];
            row[(size_t)i * words + (p >> 6)] |= 1ULL << (p & 63);
        }
    }

    for (int i = 1; i <= n; ++i) {
        const unsigned long long* ri = &row[(size_t)i * words];
        for (int j = i + 1; j <= n; ++j) {
            const unsigned long long* rj = &row[(size_t)j * words];
            int min_both = -1, max_only_i = -1;
            for (int w = 0; w < words; ++w) {
                unsigned long long both = ri[w] & rj[w];
                if (both && min_both < 0) {
                    int b = 0;
                    while (!((both >> b) & 1ULL)) ++b;
                    min_both = (w << 6) + b;
                }
                unsigned long long only = ri[w] & ~rj[w];
                if (only) {
                    int b = 63;
                    while (!((only >> b) & 1ULL)) --b;
                    max_only_i = (w << 6) + b;
                }
            }
            if (min_both >= 0 && max_only_i > min_both) return false;
        }
    }
    return true;
}

} // namespace detail_elimination

/**
 * @brief Computes a strong elimination ordering
 * @param g Input graph
 * @return StrongEliminationResult; success is false exactly when g is not
 *         strongly chordal
 *
 * Farber's construction: at every step the strict closed-neighbourhood
 * inclusions of the remaining graph are accumulated into a partial order
 * (kept transitively closed, so the result does not depend on the order the
 * relations are discovered in), and the simple vertex eliminated is one that
 * is minimal in it. Removing an arbitrary simple vertex would still recognize
 * the class but need not give a strong elimination ordering.
 *
 * Runs in O(n^4): each of the n steps recomputes the inclusions and their
 * closure.
 */
inline StrongEliminationResult compute_strong_elimination_ordering(const Graph& g) {
    StrongEliminationResult res;
    int n = g.n;
    res.number.assign(n + 1, 0);
    if (n == 0) {
        res.order.assign(1, 0);
        res.success = true;
        return res;
    }

    std::vector<unsigned char> alive(n + 1, 1);
    std::vector<int> alive_degree(n + 1, 0);
    for (int v = 1; v <= n; ++v) alive_degree[v] = (int)g.adj[v].size();

    std::vector<std::vector<unsigned char>> less(n + 1, std::vector<unsigned char>(n + 1, 0));
    std::vector<int> order(n + 1, 0);
    std::vector<int> neighbors;
    neighbors.reserve(n);

    for (int step = 1; step <= n; ++step) {
        for (int x = 1; x <= n; ++x) {
            if (!alive[x]) continue;
            for (int y = 1; y <= n; ++y) {
                if (!alive[y] || x == y) continue;
                if (detail_elimination::is_strict_closed_neighborhood_subset(
                        g, x, y, alive, alive_degree)) {
                    less[x][y] = 1;
                }
            }
        }
        for (int k = 1; k <= n; ++k) {
            if (!alive[k]) continue;
            for (int x = 1; x <= n; ++x) {
                if (!alive[x] || !less[x][k]) continue;
                for (int y = 1; y <= n; ++y) {
                    if (alive[y] && less[k][y]) less[x][y] = 1;
                }
            }
        }

        int pick = 0;
        for (int v = 1; v <= n && pick == 0; ++v) {
            if (!alive[v]) continue;
            bool minimal = true;
            for (int u = 1; u <= n && minimal; ++u) {
                if (alive[u] && less[u][v]) minimal = false;
            }
            if (!minimal) continue;
            if (detail_elimination::is_simple_vertex(g, v, alive, &neighbors)) pick = v;
        }
        if (pick == 0) {
            res.number.assign(n + 1, 0);
            return res;
        }

        order[step] = pick;
        res.number[pick] = step;
        alive[pick] = 0;
        for (size_t i = 0; i < g.adj[pick].size(); ++i) {
            int u = g.adj[pick][i];
            if (alive[u]) --alive_degree[u];
        }
    }

    if (!detail_elimination::is_strong_elimination_order(g, res.number)) {
        res.number.assign(n + 1, 0);
        return res;
    }
    res.order.swap(order);
    res.success = true;
    return res;
}

} // namespace graph_recognition

#endif
