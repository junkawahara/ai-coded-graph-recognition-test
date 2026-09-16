#ifndef GRAPH_RECOGNITION_LINE_GRAPH_H
#define GRAPH_RECOGNITION_LINE_GRAPH_H

/**
 * @file line_graph.h
 * @brief Line graph recognition
 *
 * A line graph is a graph L(H) whose vertices are the edges of some graph H,
 * where two vertices are adjacent iff the corresponding edges share an endpoint in H.
 *
 * By Krausz's theorem (1943), a line graph is characterized by edge clique covers
 * (Krausz partitions): G is a line graph iff the edge set can be partitioned
 * into cliques such that each vertex belongs to at most 2 cliques.
 * (Whitney's theorem (1932) is the different statement that H is determined by
 * L(H), with K_3 and K_{1,3} as the only exception.)
 *
 * Algorithms:
 *   - BRUTE: Krausz partition search via backtracking. The clique holding the
 *     first unassigned edge is one of at most three candidates (the common
 *     neighbourhood's clique, its isolated vertex, or the edge alone; any
 *     other shape of the common neighbourhood already proves the graph is
 *     not a line graph), so the branching is at most three-way per edge;
 *     the worst case is still exponential.
 *   - KRAUSZ: O(m * Delta) necessary condition filter + the same Krausz
 *     partition construction (default)
 *
 * The partition search recurses once per edge, so inputs with
 * m >= line_graph_recursion_limit edges are refused with std::runtime_error
 * (the recursion would overflow the default 8 MiB stack around three times
 * that many edges) -- the stack-overflow exception to the complexity policy.
 *
 * References:
 *   - Whitney (1932); Krausz (1943); Beineke (1968)
 *   - Roussopoulos (1973); Lehot (1974)
 */

#include "util/graph.h"
#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for line graph recognition
 */
enum class LineGraphAlgorithm {
    BRUTE,  /**< Krausz partition backtracking (at most three candidate cliques per edge) */
    KRAUSZ  /**< O(m * Delta) filter + Krausz partition construction (default) */
};

/**
 * @brief Largest edge count the Krausz search accepts
 *
 * The partition search recurses once per edge (depth m). With the default
 * 8 MiB stack it overflows at roughly 3 times this many edges, so
 * check_line_graph() refuses inputs with m >= this limit by throwing
 * std::runtime_error instead of crashing (the answer is then unknown).
 * Raise the process stack limit and this constant together to go further.
 */
const int line_graph_recursion_limit = 6000;

/**
 * @brief Result of line graph recognition
 */
struct LineGraphResult {
    bool is_line_graph = false; /**< true if the graph is a line graph */
    /**
     * @brief The Krausz partition: krausz[i] = vertex set of the i-th clique
     *
     * Every edge of g lies in exactly one of these cliques and every vertex in
     * at most two. Valid only when is_line_graph == true.
     */
    std::vector<std::vector<int>> krausz;
    /**
     * @brief A root graph H with L(H) = g
     *
     * Its vertices are the Krausz cliques plus one private vertex for each
     * side of g's vertices that lies in fewer than two cliques. Valid only
     * when is_line_graph == true.
     */
    Graph root_graph;
    /**
     * @brief vertex_to_root_edge[v] = the edge of root_graph that v represents (size n+1)
     *
     * Valid only when is_line_graph == true.
     */
    std::vector<std::pair<int, int>> vertex_to_root_edge;
};

namespace detail {

/**
 * @brief Turns a Krausz partition into a root graph
 * @param g The line graph
 * @param edges Its edges, in the order the partition indexes them
 * @param edge_clique edge_clique[i] = clique holding edges[i]
 * @param num_cliques Number of cliques in the partition
 * @param res Receives krausz, root_graph and vertex_to_root_edge
 * @return true if L(root_graph) really is g
 *
 * A vertex of g becomes an edge of H joining the (at most two) cliques that
 * contain it; a vertex in fewer than two cliques gets private H-vertices to
 * fill the missing endpoints, which is what makes isolated vertices and
 * pendant edges come out right. Two vertices of g cannot land on the same
 * H-edge: sharing both cliques would put the edge between them into two
 * cliques at once.
 */
inline bool build_root_graph(const Graph& g,
                             const std::vector<std::pair<int, int>>& edges,
                             const std::vector<int>& edge_clique,
                             int num_cliques, LineGraphResult& res) {
    int n = g.n;
    std::vector<std::vector<int>> cliques(num_cliques);
    std::vector<std::vector<int>> holding(n + 1);
    {
        std::vector<std::vector<char>> seen(num_cliques);
        for (int c = 0; c < num_cliques; ++c) seen[c].assign(n + 1, 0);
        for (size_t i = 0; i < edges.size(); ++i) {
            int c = edge_clique[i];
            if (c < 0 || c >= num_cliques) return false;
            int ends[2] = {edges[i].first, edges[i].second};
            for (int k = 0; k < 2; ++k) {
                if (seen[c][ends[k]]) continue;
                seen[c][ends[k]] = 1;
                cliques[c].push_back(ends[k]);
                holding[ends[k]].push_back(c);
            }
        }
    }
    for (int c = 0; c < num_cliques; ++c) std::sort(cliques[c].begin(), cliques[c].end());
    for (int v = 1; v <= n; ++v) {
        if (holding[v].size() > 2) return false;
        std::sort(holding[v].begin(), holding[v].end());
    }

    // H-vertices: 1..num_cliques are the cliques, then the private ones.
    int next_vertex = num_cliques;
    std::vector<std::pair<int, int>> root_edges;
    root_edges.reserve(n);
    std::vector<std::pair<int, int>> mapping(n + 1, std::make_pair(0, 0));
    for (int v = 1; v <= n; ++v) {
        int a = holding[v].size() > 0 ? holding[v][0] + 1 : ++next_vertex;
        int b = holding[v].size() > 1 ? holding[v][1] + 1 : ++next_vertex;
        mapping[v] = std::make_pair(a, b);
        root_edges.push_back(std::make_pair(a, b));
    }

    Graph h(next_vertex, root_edges);
    // L(h) must be g: two vertices are adjacent exactly when their edges of h
    // share an endpoint.
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            const std::pair<int, int>& a = mapping[u];
            const std::pair<int, int>& b = mapping[v];
            bool shares = a.first == b.first || a.first == b.second ||
                          a.second == b.first || a.second == b.second;
            if (shares != g.has_edge(u, v)) return false;
        }
    }

    res.krausz.swap(cliques);
    res.root_graph = h;
    res.vertex_to_root_edge.swap(mapping);
    return true;
}

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
 * @throws std::runtime_error if m >= line_graph_recursion_limit (result unknown)
 */
inline LineGraphResult check_line_graph_brute(const Graph& g) {
    LineGraphResult res;
    int n = g.n;

    if (n == 0) {
        res.vertex_to_root_edge.assign(1, std::make_pair(0, 0));
        res.is_line_graph = true;
        return res;
    }

    std::vector<std::pair<int,int>> edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back(std::make_pair(u, v));
        }
    }

    int m = (int)edges.size();
    if (m == 0) {
        // Every vertex is its own H-edge between two private H-vertices.
        res.is_line_graph = build_root_graph(g, edges, std::vector<int>(), 0, res);
        return res;
    }
    if (m >= line_graph_recursion_limit) {
        throw std::runtime_error(
            "line_graph: the Krausz partition search recurses once per edge and m = " +
            std::to_string(m) + " reaches the recursion limit " +
            std::to_string(line_graph_recursion_limit) +
            " (it would overflow the default stack)");
    }

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

            // Common neighbours of eu and ev, whatever their clique counts.
            std::vector<int> common;
            for (size_t i = 0; i < g->adj[eu].size(); ++i) {
                int w = g->adj[eu][i];
                if (w != ev && g->has_edge(ev, w)) common.push_back(w);
            }
            int nc = (int)common.size();

            // The clique of the partition holding (eu, ev) is {eu, ev} plus a
            // subset S of common. Two facts cut the 2^nc subsets down to at
            // most three candidates:
            //
            //  - In a line graph L(H) with eu = pa and ev = pb, the common
            //    neighbours are the other edges at p (a clique K) plus the
            //    edge ab if H has it, which meets no member of K. So
            //    G[common] is a clique plus at most one isolated vertex (two
            //    isolated vertices when |K| = 1); any other shape proves G is
            //    not a line graph, whatever the partial partition is.
            //  - A partition clique C with |C| >= 3 is a maximal clique of G:
            //    a vertex x outside C adjacent to all of C would need a
            //    distinct second clique for each of its |C| edges into C (two
            //    of them sharing a clique would put an edge of C into two
            //    cliques), so x would lie in at least three cliques.
            //
            // Hence S is empty, or a maximal clique of G[common]: K or the
            // isolated vertex. Trying those three (largest first) is complete.
            std::vector<int> degree(nc, 0);
            for (int i = 0; i < nc; ++i) {
                for (int j = i + 1; j < nc; ++j) {
                    if (g->has_edge(common[i], common[j])) {
                        ++degree[i];
                        ++degree[j];
                    }
                }
            }
            std::vector<int> clique_part, isolated;
            for (int i = 0; i < nc; ++i) {
                if (degree[i] == 0) isolated.push_back(common[i]);
                else clique_part.push_back(common[i]);
            }
            int kc = (int)clique_part.size();
            for (int i = 0; i < nc; ++i) {
                if (degree[i] != 0 && degree[i] != kc - 1) return false;
            }
            std::vector<std::vector<int>> candidates;
            if (isolated.size() > 1) {
                if (kc != 0 || isolated.size() != 2) return false;
                candidates.push_back(std::vector<int>(1, isolated[0]));
                candidates.push_back(std::vector<int>(1, isolated[1]));
            } else {
                if (kc > 0) candidates.push_back(clique_part);
                if (!isolated.empty()) candidates.push_back(isolated);
            }
            candidates.push_back(std::vector<int>());

            for (size_t ci = 0; ci < candidates.size(); ++ci) {
                std::vector<int> clique_verts;
                clique_verts.push_back(eu);
                clique_verts.push_back(ev);
                bool usable = true;
                for (size_t i = 0; i < candidates[ci].size(); ++i) {
                    int w = candidates[ci][i];
                    if ((*vertex_clique_count)[w] >= 2) {
                        usable = false;
                        break;
                    }
                    clique_verts.push_back(w);
                }
                if (!usable) continue;

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

    if (!state.solve(0)) return res;
    res.is_line_graph = build_root_graph(g, edges, edge_clique, num_cliques, res);
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

    if (n == 0) return check_line_graph_brute(g);

    int m_count = 0;
    for (int v = 1; v <= n; ++v) {
        m_count += (int)g.adj[v].size();
    }
    m_count /= 2;

    if (m_count == 0) return check_line_graph_brute(g);

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
 *
 * @throws std::runtime_error if the graph has line_graph_recursion_limit or
 *         more edges (the partition search would overflow the stack; the
 *         KRAUSZ filter may still answer NO before reaching it)
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
