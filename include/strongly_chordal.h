#ifndef GRAPH_RECOGNITION_STRONGLY_CHORDAL_H
#define GRAPH_RECOGNITION_STRONGLY_CHORDAL_H

/**
 * @file strongly_chordal.h
 * @brief Strongly chordal graph recognition
 *
 * Algorithm:
 *   - STRONG_ELIMINATION: Full-scan simple vertex elimination O(n^4)
 *   - PEO_MATRIX: Full-scan simple vertex elimination using adjacency-set
 *     edge queries, worst case O(n*m*Delta) (enum name is historical; no
 *     adjacency matrix is built)
 *   - MCS_SEO: Simple vertex elimination with degree-sorted inclusion checks
 *     via adjacency-list traversal, worst case O(n*m*Delta) (default)
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for strongly chordal graph recognition
 */
enum class StronglyChordalAlgorithm {
    STRONG_ELIMINATION, /**< Full-scan simple vertex elimination O(n^4) */
    PEO_MATRIX,         /**< Full-scan simple vertex elimination, adjacency-set edge queries (name is historical) */
    MCS_SEO             /**< Simple vertex elimination with degree-sorted inclusion checks (default) */
};

/**
 * @brief Result of strongly chordal graph recognition
 */
struct StronglyChordalResult {
    bool is_strongly_chordal = false; /**< true if the graph is a strongly chordal graph */
};

namespace detail_strongly_chordal {

/** @brief Enumerate neighbors of v in the alive subgraph */
inline void collect_alive_neighbors(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    neighbors->clear();
    for (size_t i = 0; i < g.adj[v].size(); ++i) {
        int u = g.adj[v][i];
        if (alive[u]) neighbors->push_back(u);
    }
}

/** @brief Determines whether N[x] is contained in N[y] in the alive subgraph */
inline bool is_closed_neighborhood_subset(
    const Graph& g,
    int x,
    int y,
    const std::vector<unsigned char>& alive) {
    // x in N[x] must be in N[y], so x==y or xy is an edge.
    if (x != y && !g.has_edge(x, y)) return false;

    for (size_t i = 0; i < g.adj[x].size(); ++i) {
        int z = g.adj[x][i];
        if (!alive[z]) continue;
        if (z == y) continue;
        if (!g.has_edge(y, z)) return false;
    }
    return true;
}

/** @brief Determines whether v is a simple vertex in the alive subgraph */
inline bool is_simple_vertex(
    const Graph& g,
    int v,
    const std::vector<unsigned char>& alive,
    std::vector<int>* neighbors) {
    collect_alive_neighbors(g, v, alive, neighbors);

    // A simple vertex must be simplicial.
    for (size_t i = 0; i < neighbors->size(); ++i) {
        int x = (*neighbors)[i];
        for (size_t j = i + 1; j < neighbors->size(); ++j) {
            int y = (*neighbors)[j];
            if (!g.has_edge(x, y)) return false;
        }
    }

    // Closed neighborhoods of neighbors are pairwise comparable by inclusion.
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

} // namespace detail_strongly_chordal

/** @brief Full-scan simple vertex elimination (original algorithm) */
inline StronglyChordalResult check_strongly_chordal_elimination(const Graph& g) {
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    std::vector<unsigned char> alive(g.n + 1, 1);
    std::vector<int> neighbors;
    neighbors.reserve(g.n);

    int remaining = g.n;
    while (remaining > 0) {
        int pick = 0;
        for (int v = 1; v <= g.n; ++v) {
            if (!alive[v]) continue;
            if (detail_strongly_chordal::is_simple_vertex(g, v, alive, &neighbors)) {
                pick = v;
                break;
            }
        }
        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
    }

    res.is_strongly_chordal = true;
    return res;
}

/**
 * @brief Full-scan simple vertex elimination, worst case O(n*m*Delta)
 *
 * 1. Chordal check (bucket MCS + PEO verification): O(n+m)
 * 2. Repeatedly scan all alive vertices for a simple vertex and remove it.
 *    Simplicial/inclusion checks use adjacency-set edge queries and
 *    adjacency-list traversal (no adjacency matrix is built).
 */
inline StronglyChordalResult check_strongly_chordal_peo_matrix(const Graph& g) {
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    int n = g.n;
    if (n <= 1) { res.is_strongly_chordal = true; return res; }

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    std::vector<unsigned char> alive(n + 1, 1);
    std::vector<int> alive_deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        alive_deg[v] = (int)g.adj[v].size();
    }

    int remaining = n;
    std::vector<int> nbrs;
    nbrs.reserve(n);

    while (remaining > 0) {
        int pick = 0;
        for (int v = 1; v <= n && pick == 0; ++v) {
            if (!alive[v]) continue;

            nbrs.clear();
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (alive[u]) nbrs.push_back(u);
            }

            bool simplicial = true;
            for (size_t a = 0; a < nbrs.size() && simplicial; ++a) {
                for (size_t b = a + 1; b < nbrs.size() && simplicial; ++b) {
                    if (!g.has_edge(nbrs[a], nbrs[b])) simplicial = false;
                }
            }
            if (!simplicial) continue;

            std::sort(nbrs.begin(), nbrs.end(), [&](int a, int b) {
                return alive_deg[a] < alive_deg[b];
            });

            bool simple = true;
            for (size_t j = 0; j + 1 < nbrs.size() && simple; ++j) {
                int x = nbrs[j], y = nbrs[j + 1];
                for (size_t k = 0; k < g.adj[x].size(); ++k) {
                    int u = g.adj[x][k];
                    if (!alive[u]) continue;
                    if (u == y) continue;
                    if (!g.has_edge(y, u)) { simple = false; break; }
                }
            }
            if (!simple) continue;

            pick = v;
        }

        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
        for (size_t j = 0; j < g.adj[pick].size(); ++j) {
            int u = g.adj[pick][j];
            if (alive[u]) alive_deg[u]--;
        }
    }

    res.is_strongly_chordal = true;
    return res;
}

/**
 * @brief Simple vertex elimination with degree-sorted inclusion checks,
 *        worst case O(n*m*Delta)
 *
 * Repeatedly removes simple vertices from a chordal graph.
 *
 * 1. Chordal check: O(n + m)
 * 2. Scan all vertices and remove simple vertices.
 *    - Simplicial check: O(deg^2) adjacency-set edge queries
 *    - Simple check: after sorting neighbors by ascending alive_deg, verify
 *      closed neighborhood inclusion of consecutive pairs by adjacency-list
 *      traversal (O(deg) edge queries per pair)
 *    - Not a strongly chordal graph if no simple vertex is found
 *
 * Differences from PEO_MATRIX:
 *   - By verifying only consecutive pairs in the simple check (after the
 *     alive_deg sort), reduces O(deg^2) all-pair comparisons to O(deg) pairs.
 */
inline StronglyChordalResult check_strongly_chordal_mcs_seo(const Graph& g) {
    StronglyChordalResult res;
    res.is_strongly_chordal = false;

    int n = g.n;
    if (n <= 1) { res.is_strongly_chordal = true; return res; }

    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) return res;

    std::vector<unsigned char> alive(n + 1, 1);
    std::vector<int> alive_deg(n + 1, 0);
    for (int v = 1; v <= n; ++v) {
        alive_deg[v] = (int)g.adj[v].size();
    }

    std::vector<int> nbrs;
    nbrs.reserve(n);

    int remaining = n;
    while (remaining > 0) {
        int pick = 0;

        for (int v = 1; v <= n && pick == 0; ++v) {
            if (!alive[v]) continue;

            nbrs.clear();
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                int u = g.adj[v][j];
                if (alive[u]) nbrs.push_back(u);
            }

            bool simplicial = true;
            for (size_t a = 0; a < nbrs.size() && simplicial; ++a) {
                for (size_t b = a + 1; b < nbrs.size() && simplicial; ++b) {
                    if (!g.has_edge(nbrs[a], nbrs[b])) simplicial = false;
                }
            }
            if (!simplicial) continue;

            if (nbrs.size() <= 1) {
                pick = v;
                break;
            }

            std::sort(nbrs.begin(), nbrs.end(), [&](int a, int b) {
                return alive_deg[a] < alive_deg[b];
            });

            // Simple check: verify N_closed[x] ⊆ N_closed[y] by adjacency list traversal
            bool simple = true;
            for (size_t j = 0; j + 1 < nbrs.size() && simple; ++j) {
                int x = nbrs[j], y = nbrs[j + 1];
                // x itself must be in N_closed[y] (x==y or edge(x,y)) -- guaranteed since simplicial
                for (size_t k = 0; k < g.adj[x].size() && simple; ++k) {
                    int w = g.adj[x][k];
                    if (!alive[w]) continue;
                    if (w == y) continue;
                    if (!g.has_edge(y, w)) simple = false;
                }
            }
            if (!simple) continue;

            pick = v;
        }

        if (pick == 0) return res;

        alive[pick] = 0;
        remaining--;
        for (size_t j = 0; j < g.adj[pick].size(); ++j) {
            int u = g.adj[pick][j];
            if (alive[u]) alive_deg[u]--;
        }
    }

    res.is_strongly_chordal = true;
    return res;
}

/**
 * @brief Determines whether the graph is a strongly chordal graph
 * @param g Input graph
 * @param algo Algorithm to use (default: MCS_SEO)
 * @return StronglyChordalResult
 */
inline StronglyChordalResult check_strongly_chordal(const Graph& g,
    StronglyChordalAlgorithm algo = StronglyChordalAlgorithm::MCS_SEO) {
    switch (algo) {
        case StronglyChordalAlgorithm::STRONG_ELIMINATION:
            return check_strongly_chordal_elimination(g);
        case StronglyChordalAlgorithm::PEO_MATRIX:
            return check_strongly_chordal_peo_matrix(g);
        case StronglyChordalAlgorithm::MCS_SEO:
            return check_strongly_chordal_mcs_seo(g);
        default:
            break;
    }
    return StronglyChordalResult();
}

} // namespace graph_recognition

#endif
