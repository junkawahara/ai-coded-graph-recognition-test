#ifndef GRAPH_RECOGNITION_GEM_FREE_H
#define GRAPH_RECOGNITION_GEM_FREE_H

/**
 * @file gem_free.h
 * @brief Gem-free graph recognition
 *
 * A gem-free graph is a graph that does not contain a gem as an induced subgraph.
 * Gem (fan F_{1,4}) is a 5-vertex, 7-edge graph formed by adding a universal vertex v
 * to P4 {a,b,c,d}. Edges: {va,vb,vc,vd,ab,bc,cd}, non-edges: {ac,ad,bd}.
 *
 * Algorithms:
 *   - BRUTE: all 5-subsets check O(n^5)
 *   - NEIGHBOR_P4_SEARCH: P4 search in each vertex's neighborhood,
 *     worst case O(n*Delta^4) (default)
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
 * @brief Algorithm selection for gem-free graph recognition
 */
enum class GemFreeAlgorithm {
    BRUTE,              /**< All 5-subsets check O(n^5) */
    NEIGHBOR_P4_SEARCH  /**< Neighbor P4 search, worst case O(n*Delta^4) (default) */
};

/**
 * @brief Result of gem-free graph recognition
 */
struct GemFreeResult {
    bool is_gem_free = false; /**< true if the graph is gem-free */
    Obstruction obstruction; /**< NO certificate: a GEM. Valid only when
                                  is_gem_free == false; filled by every variant */
};

namespace detail {

/**
 * @brief Induced gem detection by checking all 5-subsets
 *
 * Examines the edge count and degree sequence for all C(n,5) 5-subsets.
 * Gem is the unique graph on 5 vertices with (edge count=7, degree sequence={2,2,3,3,4}).
 * Complexity: O(n^5)
 */
inline GemFreeResult check_gem_free_brute(const Graph& g) {
    GemFreeResult res;
    res.is_gem_free = true;

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
                        if (edge_count != 7) continue;

                        // Gem degree sequence is {2,2,3,3,4} (sorted)
                        int sorted_deg[5];
                        for (int i = 0; i < 5; ++i) sorted_deg[i] = deg[i];
                        for (int i = 0; i < 4; ++i)
                            for (int j = i + 1; j < 5; ++j)
                                if (sorted_deg[i] > sorted_deg[j]) {
                                    int tmp = sorted_deg[i];
                                    sorted_deg[i] = sorted_deg[j];
                                    sorted_deg[j] = tmp;
                                }

                        if (sorted_deg[0] == 2 && sorted_deg[1] == 2 &&
                            sorted_deg[2] == 3 && sorted_deg[3] == 3 &&
                            sorted_deg[4] == 4) {
                            res.is_gem_free = false;
                            res.obstruction = make_obstruction(
                                ObstructionKind::GEM, detail_obstruction::find_gem(g));
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
 * @brief Gem detection by neighbor P4 search
 *
 * Since gem = P4 + universal vertex, for each vertex v,
 * searches whether an induced P4 exists in G[N(v)].
 * Complexity: worst case O(n * Delta^4) (the P4 search enumerates up to
 * four nested neighborhood loops per vertex)
 */
inline GemFreeResult check_gem_free_neighbor_p4_search(const Graph& g) {
    GemFreeResult res;
    res.is_gem_free = true;

    int n = g.n;
    if (n < 5) return res;

    for (int v = 1; v <= n; ++v) {
        if (g.adj[v].size() < 4) continue;

        // Search for induced P4 a-b-c-d in G[N(v)]
        for (size_t bi = 0; bi < g.adj[v].size(); ++bi) {
            int b = g.adj[v][bi];
            for (size_t ci = 0; ci < g.adj[b].size(); ++ci) {
                int c = g.adj[b][ci];
                if (c == v) continue;
                if (!g.has_edge(v, c)) continue; // c must be in N(v)

                // Edge b-c exists, both in N(v)
                // a in N(v) ∩ N(b), a != c, a not adj c
                for (size_t ai = 0; ai < g.adj[v].size(); ++ai) {
                    int a = g.adj[v][ai];
                    if (a == b || a == c) continue;
                    if (!g.has_edge(a, b)) continue;
                    if (g.has_edge(a, c)) continue;

                    // Induced P3: a-b-c in N(v)
                    // d in N(v) ∩ N(c), d != b, d != a, d not adj b, d not adj a
                    for (size_t di = 0; di < g.adj[v].size(); ++di) {
                        int d = g.adj[v][di];
                        if (d == b || d == c || d == a) continue;
                        if (!g.has_edge(d, c)) continue;
                        if (g.has_edge(d, b)) continue;
                        if (g.has_edge(d, a)) continue;

                        // Gem {v, a, b, c, d} found
                        res.is_gem_free = false;
                        std::vector<int> vs;
                        vs.push_back(a);
                        vs.push_back(b);
                        vs.push_back(c);
                        vs.push_back(d);
                        vs.push_back(v);
                        res.obstruction = make_obstruction(ObstructionKind::GEM, vs);
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
 * @brief Determines whether the graph is gem-free
 * @param g Input graph
 * @param algo Algorithm to use (default: NEIGHBOR_P4_SEARCH)
 * @return GemFreeResult
 *
 * G is gem-free iff it does not contain a gem as an induced subgraph.
 * Gem is P4 {a,b,c,d} + universal vertex v (5 vertices, 7 edges).
 */
inline GemFreeResult check_gem_free(const Graph& g,
    GemFreeAlgorithm algo = GemFreeAlgorithm::NEIGHBOR_P4_SEARCH) {
    switch (algo) {
        case GemFreeAlgorithm::BRUTE:
            return detail::check_gem_free_brute(g);
        case GemFreeAlgorithm::NEIGHBOR_P4_SEARCH:
            return detail::check_gem_free_neighbor_p4_search(g);
        default:
            break;
    }
    return GemFreeResult();
}

} // namespace graph_recognition

#endif
