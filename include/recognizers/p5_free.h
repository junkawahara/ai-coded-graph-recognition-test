#ifndef GRAPH_RECOGNITION_P5_FREE_H
#define GRAPH_RECOGNITION_P5_FREE_H

/**
 * @file p5_free.h
 * @brief P5-free graph recognition
 *
 * A P5-free graph is a graph that does not contain P5 (path of length 4)
 * as an induced subgraph. P5 is a graph on 5 vertices {a,b,c,d,e} with edges {ab,bc,cd,de}.
 *
 * Extends the Pk-free series: P3-free = cluster (done), P4-free = cograph (done).
 *
 * Algorithms:
 *   - BRUTE: all 5-subsets check O(n^5)
 *   - PATH_SEARCH: path extension from each edge to search for P5 (default)
 *
 * References:
 *   - Brandstädt, Le, Spinrad, "Graph Classes: A Survey," SIAM, 1999
 */

#include "certificates/forbidden_subgraph.h"
#include "util/graph.h"
#include "certificates/obstruction_extract.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for P5-free graph recognition
 */
enum class P5FreeAlgorithm {
    BRUTE,       /**< All 5-subsets check O(n^5) */
    PATH_SEARCH  /**< Path extension search (default) */
};

/**
 * @brief Result of P5-free graph recognition
 */
struct P5FreeResult {
    bool is_p5_free = false; /**< true if the graph is P5-free */
    Obstruction obstruction; /**< NO certificate: a P5. Valid only when
                                  is_p5_free == false; filled by every variant */
};

namespace detail {

/**
 * @brief Induced P5 detection by checking all 5-subsets
 *
 * Examines the edge count and degree sequence for all C(n,5) 5-subsets.
 * Both P5 and K3+K2 have (edge count=4, degree sequence={1,1,2,2,2});
 * a connectivity check distinguishes them (P5 is the connected one).
 * Complexity: O(n^5)
 */
inline P5FreeResult check_p5_free_brute(const Graph& g) {
    P5FreeResult res;
    res.is_p5_free = true;

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
                        if (edge_count != 4) continue;

                        // P5 degree sequence is {1,1,2,2,2} (sorted)
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
                            sorted_deg[2] == 2 && sorted_deg[3] == 2 &&
                            sorted_deg[4] == 2) {
                            // Degree sequence matches P5, but also matches
                            // K3+K2 (triangle + edge). Check connectivity
                            // to distinguish: P5 is connected, K3+K2 is not.
                            // BFS from v[0] using only induced edges.
                            int visited = 0;
                            unsigned char vis5[5] = {0, 0, 0, 0, 0};
                            int queue5[5];
                            queue5[0] = 0;
                            vis5[0] = 1;
                            int qh = 0, qt = 1;
                            while (qh < qt) {
                                int cur = queue5[qh++];
                                ++visited;
                                for (int nb = 0; nb < 5; ++nb) {
                                    if (!vis5[nb] && g.has_edge(v[cur], v[nb])) {
                                        vis5[nb] = 1;
                                        queue5[qt++] = nb;
                                    }
                                }
                            }
                            if (visited == 5) {
                                res.is_p5_free = false;
                                res.obstruction = make_obstruction(
                                    ObstructionKind::P5, detail_obstruction::find_p5(g));
                                return res;
                            }
                        }
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief Induced P5 detection via path extension search
 *
 * For each edge (b,c), extends a-b-c to an induced P3,
 * then further to a-b-c-d, a-b-c-d-e to search for induced P5.
 */
inline P5FreeResult check_p5_free_path_search(const Graph& g) {
    P5FreeResult res;
    res.is_p5_free = true;

    int n = g.n;
    if (n < 5) return res;

    // For each directed edge b->c, search for induced P5: a-b-c-d-e
    for (int b = 1; b <= n; ++b) {
        for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
            int c = g.adj[b][ci];

            // a: N(b) \ {c}, a not adj c -> induced P3: a-b-c
            for (size_t ai = 0; ai < g.adj[b].size(); ++ai) {
                int a = g.adj[b][ai];
                if (a == c) continue;
                if (g.has_edge(a, c)) continue;

                // d: N(c) \ {b}, d not adj b, d not adj a -> induced P4: a-b-c-d
                for (size_t di = 0; di < g.adj[c].size(); ++di) {
                    int d = g.adj[c][di];
                    if (d == b || d == a) continue;
                    if (g.has_edge(d, b)) continue;
                    if (g.has_edge(d, a)) continue;

                    // e: N(d) \ {c}, e not adj c, e not adj b, e not adj a
                    // -> induced P5: a-b-c-d-e
                    for (size_t ei = 0; ei < g.adj[d].size(); ++ei) {
                        int e = g.adj[d][ei];
                        if (e == c || e == b || e == a) continue;
                        if (g.has_edge(e, c)) continue;
                        if (g.has_edge(e, b)) continue;
                        if (g.has_edge(e, a)) continue;

                        // Induced P5: a-b-c-d-e found
                        res.is_p5_free = false;
                        std::vector<int> vs;
                        vs.push_back(a);
                        vs.push_back(b);
                        vs.push_back(c);
                        vs.push_back(d);
                        vs.push_back(e);
                        res.obstruction = make_obstruction(ObstructionKind::P5, vs);
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
 * @brief Determines whether the graph is P5-free
 * @param g Input graph
 * @param algo Algorithm to use (default: PATH_SEARCH)
 * @return P5FreeResult
 *
 * G is P5-free iff it does not contain P5 (path of length 4) as an induced subgraph.
 * P5 is a graph on 5 vertices {a,b,c,d,e} with edges {ab,bc,cd,de}.
 * All graphs with n <= 4 are P5-free.
 */
inline P5FreeResult check_p5_free(const Graph& g,
    P5FreeAlgorithm algo = P5FreeAlgorithm::PATH_SEARCH) {
    switch (algo) {
        case P5FreeAlgorithm::BRUTE:
            return detail::check_p5_free_brute(g);
        case P5FreeAlgorithm::PATH_SEARCH:
            return detail::check_p5_free_path_search(g);
        default:
            break;
    }
    return P5FreeResult();
}

} // namespace graph_recognition

#endif
