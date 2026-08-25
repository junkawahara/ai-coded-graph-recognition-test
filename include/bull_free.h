#ifndef GRAPH_RECOGNITION_BULL_FREE_H
#define GRAPH_RECOGNITION_BULL_FREE_H

/**
 * @file bull_free.h
 * @brief Bull-free graph recognition
 *
 * A bull-free graph is a graph that does not contain a bull as an induced subgraph.
 * A bull is a 5-vertex, 5-edge graph formed by adding two pendant edges a-x, b-y
 * to a triangle {a,b,c} (x,y are outside the triangle, x != y).
 *
 * Algorithms:
 *   - BRUTE: check all 5-subsets O(n^5)
 *   - TRIANGLE_SEARCH: triangle enumeration + pendant search O(m*Delta^2) (default)
 *
 * References:
 *   - Chudnovsky, "The structure of bull-free graphs I-III," JCTB, 2012
 */

#include "forbidden_subgraph.h"
#include "graph.h"
#include "obstruction_extract.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for bull-free graph recognition
 */
enum class BullFreeAlgorithm {
    BRUTE,           /**< brute-force check over all 5-subsets O(n^5) */
    TRIANGLE_SEARCH  /**< triangle + pendant search O(m*Delta^2) (default) */
};

/**
 * @brief Result of bull-free graph recognition
 */
struct BullFreeResult {
    bool is_bull_free = false; /**< true if the graph is bull-free */
    Obstruction obstruction; /**< NO certificate: a BULL. Valid only when
                                  is_bull_free == false; filled by every variant */
};

namespace detail {

/**
 * @brief Induced bull detection over all 5-subsets
 *
 * Checks edge count and degree sequence for all C(n,5) 5-subsets.
 * A bull is the unique graph on 5 vertices with edge count=5 and degree sequence {1,1,2,3,3}.
 * Complexity: O(n^5)
 */
inline BullFreeResult check_bull_free_brute(const Graph& g) {
    BullFreeResult res;
    res.is_bull_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int a = 1; a <= n - 4; ++a) {
        for (int b = a + 1; b <= n - 3; ++b) {
            for (int c = b + 1; c <= n - 2; ++c) {
                for (int d = c + 1; d <= n - 1; ++d) {
                    for (int e = d + 1; e <= n; ++e) {
                        int v[5] = {a, b, c, d, e};
                        int deg[5] = {0, 0, 0, 0, 0};
                        int edge_count = 0;
                        for (int i = 0; i < 5; ++i) {
                            for (int j = i + 1; j < 5; ++j) {
                                if (g.has_edge(v[i], v[j])) {
                                    ++deg[i];
                                    ++deg[j];
                                    ++edge_count;
                                }
                            }
                        }
                        if (edge_count != 5) continue;

                        // The degree sequence of a bull is {1,1,2,3,3} (sorted)
                        int sorted_deg[5];
                        for (int i = 0; i < 5; ++i) sorted_deg[i] = deg[i];
                        for (int i = 0; i < 4; ++i)
                            for (int j = i + 1; j < 5; ++j)
                                if (sorted_deg[i] > sorted_deg[j]) {
                                    int tmp = sorted_deg[i];
                                    sorted_deg[i] = sorted_deg[j];
                                    sorted_deg[j] = tmp;
                                }

                        if (sorted_deg[0] == 1 && sorted_deg[1] == 1 &&
                            sorted_deg[2] == 2 && sorted_deg[3] == 3 &&
                            sorted_deg[4] == 3) {
                            res.is_bull_free = false;
                            // The degree sequence identifies a bull but not
                            // which vertex plays which role; the shared finder
                            // returns them already named.
                            res.obstruction = make_obstruction(
                                ObstructionKind::BULL, detail_obstruction::find_bull(g));
                            return res;
                        }
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief Bull detection via triangle enumeration + pendant search
 *
 * Finds triangles via common neighbor c of each edge (a,b), treats c as the degree-2 vertex,
 * searches for x in N(a)\{b,c} not adjacent to b,c, and y in N(b)\{a,c,x} not adjacent
 * to a,c,x.
 * Complexity: O(m * Delta^2)
 */
inline BullFreeResult check_bull_free_triangle_search(const Graph& g) {
    BullFreeResult res;
    res.is_bull_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int a = 1; a <= n; ++a) {
        for (size_t bi = 0; bi < g.adj[a].size(); ++bi) {
            int b = g.adj[a][bi];
            if (b <= a) continue; // Process each edge only once

            // Find common neighbor c (triangle {a, b, c})
            for (size_t ci = 0; ci < g.adj[a].size(); ++ci) {
                int c = g.adj[a][ci];
                if (c == b) continue;
                if (!g.has_edge(b, c)) continue;

                // Triangle {a,b,c} found. Treat c as the degree-2 vertex.
                // x in N(a)\{b,c}: x not adj to b, x not adj to c
                for (size_t xi = 0; xi < g.adj[a].size(); ++xi) {
                    int x = g.adj[a][xi];
                    if (x == b || x == c) continue;
                    if (g.has_edge(x, b)) continue;
                    if (g.has_edge(x, c)) continue;

                    // y in N(b)\{a,c,x}: y not adj to a, y not adj to c, y not adj to x
                    for (size_t yi = 0; yi < g.adj[b].size(); ++yi) {
                        int y = g.adj[b][yi];
                        if (y == a || y == c || y == x) continue;
                        if (g.has_edge(y, a)) continue;
                        if (g.has_edge(y, c)) continue;
                        if (g.has_edge(y, x)) continue;

                        // Bull {a,b,c,x,y} found
                        res.is_bull_free = false;
                        std::vector<int> vs;
                        vs.push_back(a);
                        vs.push_back(b);
                        vs.push_back(c);
                        vs.push_back(x);
                        vs.push_back(y);
                        res.obstruction = make_obstruction(ObstructionKind::BULL, vs);
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief Determines whether a graph is bull-free
 * @param g Input graph
 * @param algo Algorithm to use (default: TRIANGLE_SEARCH)
 * @return BullFreeResult
 *
 * G is bull-free iff it does not contain a bull as an induced subgraph.
 */
inline BullFreeResult check_bull_free(const Graph& g,
    BullFreeAlgorithm algo = BullFreeAlgorithm::TRIANGLE_SEARCH) {
    switch (algo) {
        case BullFreeAlgorithm::BRUTE:
            return detail::check_bull_free_brute(g);
        case BullFreeAlgorithm::TRIANGLE_SEARCH:
            return detail::check_bull_free_triangle_search(g);
        default:
            break;
    }
    return BullFreeResult();
}

} // namespace graph_recognition

#endif
