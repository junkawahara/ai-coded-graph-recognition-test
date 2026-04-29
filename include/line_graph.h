#ifndef GRAPH_RECOGNITION_LINE_GRAPH_H
#define GRAPH_RECOGNITION_LINE_GRAPH_H

/**
 * @file line_graph.h
 * @brief Line graph recognition
 *
 * A line graph is a graph L(H) whose vertices are the edges of some graph H,
 * where two vertices are adjacent iff the corresponding edges share an endpoint in H.
 *
 * By Whitney's theorem (1932), a line graph is characterized by edge clique covers
 * (Krausz partitions): G is a line graph iff the edge set can be partitioned
 * into cliques such that each vertex belongs to at most 2 cliques.
 *
 * Algorithms:
 *   - BRUTE: Krausz partition search via backtracking (for small graphs)
 *   - KRAUSZ: O(m * Delta) necessary condition filter + Krausz partition construction (default)
 *
 * References:
 *   - Whitney (1932); Krausz (1943); Beineke (1968)
 *   - Roussopoulos (1973); Lehot (1974)
 */

#include "graph.h"
#include <unordered_map>
#include <vector>
#include <algorithm>

namespace graph_recognition {

/**
 * @brief Algorithm selection for line graph recognition
 */
enum class LineGraphAlgorithm {
    BRUTE,  /**< Krausz partition backtracking (for small graphs) */
    KRAUSZ  /**< O(m * Delta) filter + Krausz partition construction (default) */
};

/**
 * @brief Result of line graph recognition
 */
struct LineGraphResult {
    bool is_line_graph = false; /**< true if the graph is a line graph */
};

namespace detail {

// Edge list index lookup helper (hash map based, O(m) memory)
struct EdgeIndex {
    int n;
    std::unordered_map<long long, int> idx;

    EdgeIndex() : n(0) {}

    void build(const Graph& g, const std::vector<std::pair<int,int>>& edges, int m) {
        n = g.n;
        idx.reserve((size_t)m * 2 + 1);
        for (int eid = 0; eid < m; ++eid) {
            int u = edges[eid].first, v = edges[eid].second;
            idx[(long long)u * (n + 1) + v] = eid;
            idx[(long long)v * (n + 1) + u] = eid;
        }
    }

    int get(int u, int v) const {
        if (u < 1 || u > n || v < 1 || v > n) return -1;
        std::unordered_map<long long, int>::const_iterator it =
            idx.find((long long)u * (n + 1) + v);
        return it != idx.end() ? it->second : -1;
    }
};

/**
 * @brief Line graph recognition via Krausz partition backtracking
 */
inline LineGraphResult check_line_graph_brute(const Graph& g) {
    LineGraphResult res;
    res.is_line_graph = true;
    int n = g.n;

    if (n == 0) return res;

    std::vector<std::pair<int,int>> edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back(std::make_pair(u, v));
        }
    }

    int m = (int)edges.size();
    if (m == 0) return res;

    // Build edge index
    EdgeIndex eidx;
    eidx.build(g, edges, m);

    std::vector<int> edge_clique(m, -1);
    std::vector<int> vertex_clique_count(n + 1, 0);
    int num_cliques = 0;

    struct BacktrackState {
        const Graph* g;
        int n, m;
        const std::vector<std::pair<int,int>>* edges;
        const EdgeIndex* eidx;
        std::vector<int>* edge_clique;
        std::vector<int>* vertex_clique_count;
        int* num_cliques;

        bool solve(int first_unassigned) {
            while (first_unassigned < m && (*edge_clique)[first_unassigned] != -1) {
                first_unassigned++;
            }
            if (first_unassigned >= m) return true;

            int eu = (*edges)[first_unassigned].first;
            int ev = (*edges)[first_unassigned].second;

            if ((*vertex_clique_count)[eu] >= 2 || (*vertex_clique_count)[ev] >= 2) {
                return false;
            }

            std::vector<int> common;
            for (size_t i = 0; i < g->adj[eu].size(); ++i) {
                int w = g->adj[eu][i];
                if (w != ev && g->has_edge(ev, w) && (*vertex_clique_count)[w] < 2) {
                    common.push_back(w);
                }
            }

            int nc = (int)common.size();

            // Subset enumeration uses 'int mask' below, so cap nc to avoid
            // signed shift UB. Line graphs of stars and complete graphs can
            // legitimately have large common-neighbor sets (e.g. K_n = L(K_{1,n})
            // gives nc = n-2), so we must NOT reject on nc alone. Instead, when
            // the full set of common neighbors is itself a clique, use it
            // directly (mask = (1<<nc)-1 equivalent) without enumerating.
            if (nc >= 30) {
                bool full_is_clique = true;
                for (int i = 0; i < nc && full_is_clique; ++i) {
                    for (int j = i + 1; j < nc && full_is_clique; ++j) {
                        if (!g->has_edge(common[i], common[j])) {
                            full_is_clique = false;
                        }
                    }
                }
                if (!full_is_clique) {
                    // Bit-mask enumeration would overflow; treat as non-line-graph.
                    return false;
                }
                std::vector<int> clique_verts;
                clique_verts.push_back(eu);
                clique_verts.push_back(ev);
                for (int i = 0; i < nc; ++i) clique_verts.push_back(common[i]);

                std::vector<int> clique_edge_ids;
                bool all_unassigned = true;
                for (size_t i = 0; i < clique_verts.size() && all_unassigned; ++i) {
                    for (size_t j = i + 1; j < clique_verts.size() && all_unassigned; ++j) {
                        int a = clique_verts[i], b = clique_verts[j];
                        int eid = eidx->get(a, b);
                        if (eid == -1 || (*edge_clique)[eid] != -1) {
                            all_unassigned = false;
                        } else {
                            clique_edge_ids.push_back(eid);
                        }
                    }
                }
                if (!all_unassigned) return false;

                int cid = (*num_cliques)++;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = cid;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]++;
                }
                if (solve(first_unassigned + 1)) return true;

                (*num_cliques)--;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = -1;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]--;
                }
                return false;
            }
            for (int mask = (1 << nc) - 1; mask >= 0; --mask) {
                std::vector<int> clique_verts;
                clique_verts.push_back(eu);
                clique_verts.push_back(ev);
                for (int i = 0; i < nc; ++i) {
                    if (mask & (1 << i)) clique_verts.push_back(common[i]);
                }

                bool is_clique = true;
                for (size_t i = 2; i < clique_verts.size() && is_clique; ++i) {
                    for (size_t j = i + 1; j < clique_verts.size() && is_clique; ++j) {
                        if (!g->has_edge(clique_verts[i], clique_verts[j])) {
                            is_clique = false;
                        }
                    }
                }
                if (!is_clique) continue;

                std::vector<int> clique_edge_ids;
                bool all_unassigned = true;
                for (size_t i = 0; i < clique_verts.size() && all_unassigned; ++i) {
                    for (size_t j = i + 1; j < clique_verts.size() && all_unassigned; ++j) {
                        int a = clique_verts[i], b = clique_verts[j];
                        int eid = eidx->get(a, b);
                        if (eid == -1 || (*edge_clique)[eid] != -1) {
                            all_unassigned = false;
                        } else {
                            clique_edge_ids.push_back(eid);
                        }
                    }
                }
                if (!all_unassigned) continue;

                int cid = (*num_cliques)++;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = cid;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]++;
                }

                if (solve(first_unassigned + 1)) return true;

                (*num_cliques)--;
                for (size_t i = 0; i < clique_edge_ids.size(); ++i) {
                    (*edge_clique)[clique_edge_ids[i]] = -1;
                }
                for (size_t i = 0; i < clique_verts.size(); ++i) {
                    (*vertex_clique_count)[clique_verts[i]]--;
                }
            }

            return false;
        }
    };

    BacktrackState state;
    state.g = &g;
    state.n = n;
    state.m = m;
    state.edges = &edges;
    state.eidx = &eidx;
    state.edge_clique = &edge_clique;
    state.vertex_clique_count = &vertex_clique_count;
    state.num_cliques = &num_cliques;

    res.is_line_graph = state.solve(0);
    return res;
}

/**
 * @brief Line graph recognition via necessary condition filter + Krausz partition
 *
 * 1. O(m * Delta) filter: checks whether the complement of N(v) is bipartite for each vertex v.
 *    A necessary condition for line graphs; rejects most non-line-graphs here.
 * 2. Krausz partition construction: after passing the filter, builds the partition via backtracking.
 *    Fast because the filter greatly reduces the cases that reach this stage.
 *
 * Reference: Roussopoulos (1973)
 */
inline LineGraphResult check_line_graph_krausz(const Graph& g) {
    LineGraphResult res;
    res.is_line_graph = false;
    int n = g.n;

    if (n == 0) { res.is_line_graph = true; return res; }

    int m_count = 0;
    for (int v = 1; v <= n; ++v) {
        m_count += (int)g.adj[v].size();
    }
    m_count /= 2;

    if (m_count == 0) { res.is_line_graph = true; return res; }

    // Step 1: Fast filter - complement of N(v) must be bipartite for all v.
    // This is O(m * Δ) and rejects most non-line-graphs.
    for (int v = 1; v <= n; ++v) {
        int dv = (int)g.adj[v].size();
        if (dv <= 2) continue;

        const std::vector<int>& nbrs = g.adj[v];
        std::vector<int> color(dv, -1);
        bool bipartite = true;

        for (int s = 0; s < dv && bipartite; ++s) {
            if (color[s] != -1) continue;
            color[s] = 0;
            std::vector<int> q;
            q.push_back(s);
            size_t qi = 0;
            while (qi < q.size() && bipartite) {
                int ui = (int)q[qi++];
                int uc = color[ui];
                for (int ji = 0; ji < dv; ++ji) {
                    if (ji == ui) continue;
                    if (g.has_edge(nbrs[ui], nbrs[ji])) continue;
                    if (color[ji] == -1) {
                        color[ji] = 1 - uc;
                        q.push_back(ji);
                    } else if (color[ji] == uc) {
                        bipartite = false;
                    }
                }
            }
        }

        if (!bipartite) return res;
    }

    // Step 2: Build Krausz partition via backtracking.
    // The bipartiteness filter eliminates most non-line-graphs in O(m * Δ),
    // so the backtracking phase runs on a much smaller fraction of inputs.
    return check_line_graph_brute(g);
}

} // namespace detail

/**
 * @brief Determines whether the graph is a line graph
 * @param g Input graph
 * @param algo Algorithm to use (default: KRAUSZ)
 * @return LineGraphResult
 *
 * G is a line graph iff there exists a graph H such that G = L(H).
 * Equivalently, G contains none of Beineke's 9 forbidden induced subgraphs.
 * Equivalently, a Krausz partition exists.
 */
inline LineGraphResult check_line_graph(const Graph& g,
    LineGraphAlgorithm algo = LineGraphAlgorithm::KRAUSZ) {
    switch (algo) {
        case LineGraphAlgorithm::BRUTE:
            return detail::check_line_graph_brute(g);
        case LineGraphAlgorithm::KRAUSZ:
            return detail::check_line_graph_krausz(g);
        default:
            break;
    }
    return LineGraphResult();
}

} // namespace graph_recognition

#endif
