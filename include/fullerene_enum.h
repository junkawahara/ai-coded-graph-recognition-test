#ifndef GRAPH_RECOGNITION_FULLERENE_ENUM_H
#define GRAPH_RECOGNITION_FULLERENE_ENUM_H

/**
 * @file fullerene_enum.h
 * @brief Non-isomorphic enumeration of fullerene graphs
 *
 * Enumerates all non-isomorphic fullerene graphs on n vertices.
 *
 * A fullerene graph is:
 *   - Cubic: every vertex has degree 3
 *   - Planar
 *   - All faces are pentagons (12) or hexagons (n/2 - 10)
 *   - n is even, n >= 20, n != 22
 *
 * Algorithm (dual approach):
 *   The dual of a fullerene is a triangulation with vertex degrees in {5,6} (v = n/2+2 vertices).
 *   1. Enumerate v-vertex triangulations with degree constraints via reverse search
 *   2. Extract faces (all triangles) from the planar embedding of each triangulation
 *   3. Build the dual graph (= fullerene)
 *   4. Remove isomorphic duplicates using BFS canonical form
 *
 * Non-isomorphic counts: OEIS A007894
 *   1, 0, 1, 1, 2, 3, 6, 6, 15, 17, 40, ... (n=20, 22, 24, ...)
 *
 * References:
 *   Brinkmann, Goedgebeur, McKay, J. Chem. Inf. Model. 52, 2012 (buckygen)
 */

#include "graph.h"
#include "planar.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace graph_recognition {

struct FullereneEnumeratedGraph {
    int n;
    std::vector<std::pair<int, int> > edges;
};

struct FullereneEnumerationResult {
    std::vector<FullereneEnumeratedGraph> graphs;
};

enum class FullereneEnumAlgorithm {
    REVERSE_SEARCH
};

namespace detail {

// ---- State for triangulation enumeration ----
struct TriangEnumState {
    int total_v;      // Number of vertices in the triangulation
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;

    explicit TriangEnumState(int v)
        : total_v(v), alive_count(0),
          adj(v + 1, std::vector<char>(v + 1, 0)),
          deg(v + 1, 0) {}
};

inline int triang_count_components(const TriangEnumState& state, int x) {
    std::vector<char> visited(x + 1, 0);
    int comp = 0;
    for (int s = 1; s <= x; ++s) {
        if (visited[s]) continue;
        comp++;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
        for (std::size_t qi = 0; qi < queue.size(); ++qi) {
            int v = queue[qi];
            for (int u = 1; u <= x; ++u) {
                if (!visited[u] && state.adj[v][u]) {
                    visited[u] = 1;
                    queue.push_back(u);
                }
            }
        }
    }
    return comp;
}

// ---- Extract faces from triangulation ----

/**
 * @brief Extracts faces of a triangulation
 *
 * In a triangulation (maximal planar graph), the neighbors of each vertex form a cycle.
 * Builds a rotation system from the cycle order and enumerates faces via dart tracing.
 *
 * @return List of faces (each face is a vertex sequence), empty on failure
 */
inline std::vector<std::vector<int> > extract_triangulation_faces(
    int v,
    const std::vector<std::vector<char> >& adj) {

    // Build adjacency list
    std::vector<std::vector<int> > adj_list(v + 1);
    for (int u = 1; u <= v; ++u)
        for (int w = u + 1; w <= v; ++w)
            if (adj[u][w]) {
                adj_list[u].push_back(w);
                adj_list[w].push_back(u);
            }

    // Compute rotation system for each vertex:
    // In a triangulation, the neighbors of each vertex w correspond to
    // faces incident to w, and the induced subgraph of neighbors forms a cycle.
    // next_cw[w][u] = next clockwise neighbor after u in the rotation system of w
    std::vector<std::map<int, int> > next_cw(v + 1);

    for (int w = 1; w <= v; ++w) {
        const std::vector<int>& nbrs = adj_list[w];
        int d = (int)nbrs.size();
        if (d < 3) return std::vector<std::vector<int> >();

        // Build cycle from edges among nbrs
        // Each neighbor is adjacent to exactly 2 other vertices in nbrs (forming a cycle)
        std::vector<std::vector<int> > nbr_adj(d);
        std::map<int, int> nbr_idx;
        for (int i = 0; i < d; ++i) nbr_idx[nbrs[i]] = i;

        for (int i = 0; i < d; ++i) {
            for (int j = i + 1; j < d; ++j) {
                if (adj[nbrs[i]][nbrs[j]]) {
                    nbr_adj[i].push_back(j);
                    nbr_adj[j].push_back(i);
                }
            }
        }

        // Trace the cycle
        std::vector<int> cycle;
        std::vector<char> used(d, 0);
        cycle.push_back(0);
        used[0] = 1;
        int prev = -1, cur = 0;
        for (int step = 1; step < d; ++step) {
            bool found = false;
            for (std::size_t ni = 0; ni < nbr_adj[cur].size(); ++ni) {
                int nxt = nbr_adj[cur][ni];
                if (nxt != prev && !used[nxt]) {
                    cycle.push_back(nxt);
                    used[nxt] = 1;
                    prev = cur;
                    cur = nxt;
                    found = true;
                    break;
                }
            }
            if (!found) return std::vector<std::vector<int> >();
        }

        // The cycle order is a rotation (clockwise or counterclockwise)
        // Set next_cw[w]
        for (int i = 0; i < d; ++i) {
            int a = nbrs[cycle[i]];
            int b = nbrs[cycle[(i + 1) % d]];
            next_cw[w][a] = b;
        }
    }

    // Extract faces via dart tracing
    // dart (u, w) -> next dart (w, next_cw[w][u])
    std::set<std::pair<int, int> > visited_darts;
    std::vector<std::vector<int> > faces;

    for (int u = 1; u <= v; ++u) {
        for (std::size_t ni = 0; ni < adj_list[u].size(); ++ni) {
            int w = adj_list[u][ni];
            if (visited_darts.count(std::make_pair(u, w))) continue;

            std::vector<int> face;
            int cu = u, cw = w;
            int max_steps = 3 * v;
            while (max_steps-- > 0) {
                if (visited_darts.count(std::make_pair(cu, cw))) break;
                visited_darts.insert(std::make_pair(cu, cw));
                face.push_back(cu);
                int nxt = next_cw[cw][cu];
                cu = cw;
                cw = nxt;
            }
            if (max_steps <= 0) return std::vector<std::vector<int> >();
            if ((int)face.size() != 3) {
                // In a triangulation, all faces should be triangles.
                // One orientation failed; simply return as invalid.
                return std::vector<std::vector<int> >();
            }
            faces.push_back(face);
        }
    }

    // Euler formula verification: V - E + F = 2
    int E = 0;
    for (int u = 1; u <= v; ++u) E += (int)adj_list[u].size();
    E /= 2;
    if (v - E + (int)faces.size() != 2) {
        return std::vector<std::vector<int> >();
    }

    return faces;
}

/**
 * @brief Builds the dual graph (fullerene) from the face list of a triangulation
 */
inline bool build_dual_fullerene(
    int v,
    const std::vector<std::vector<char> >& triang_adj,
    const std::vector<std::vector<int> >& faces,
    int fullerene_n,
    std::vector<std::pair<int, int> >& fullerene_edges) {

    int f = (int)faces.size();
    if (f != fullerene_n) return false;

    // Edge to face mapping
    // Each edge belongs to exactly 2 faces
    std::map<std::pair<int, int>, std::vector<int> > edge_to_faces;
    for (int fi = 0; fi < f; ++fi) {
        const std::vector<int>& face = faces[fi];
        int fs = (int)face.size();
        for (int j = 0; j < fs; ++j) {
            int a = face[j], b = face[(j + 1) % fs];
            int lo = (a < b) ? a : b;
            int hi = (a < b) ? b : a;
            edge_to_faces[std::make_pair(lo, hi)].push_back(fi);
        }
    }

    // Build dual graph edges (face numbers are 1-indexed)
    std::set<std::pair<int, int> > dual_edges;
    for (std::map<std::pair<int, int>, std::vector<int> >::const_iterator
             it = edge_to_faces.begin(); it != edge_to_faces.end(); ++it) {
        const std::vector<int>& flist = it->second;
        if ((int)flist.size() != 2) return false;
        int a = flist[0] + 1; // 1-indexed
        int b = flist[1] + 1;
        if (a > b) { int tmp = a; a = b; b = tmp; }
        dual_edges.insert(std::make_pair(a, b));
    }

    fullerene_edges.assign(dual_edges.begin(), dual_edges.end());
    std::sort(fullerene_edges.begin(), fullerene_edges.end());
    return true;
}

// ---- BFS canonical form ----

inline std::string fullerene_bfs_code(
    int n,
    const std::vector<std::vector<int> >& adj_list,
    int start) {

    std::vector<int> label(n + 1, -1);
    std::vector<int> order;
    order.reserve(n);
    label[start] = 0;
    order.push_back(start);

    for (std::size_t qi = 0; qi < order.size(); ++qi) {
        int v = order[qi];
        std::vector<std::pair<int, int> > nbrs;
        for (std::size_t i = 0; i < adj_list[v].size(); ++i) {
            int u = adj_list[v][i];
            if (label[u] == -1) {
                int min_lbl = label[v];
                for (std::size_t j = 0; j < adj_list[u].size(); ++j) {
                    int w = adj_list[u][j];
                    if (label[w] >= 0 && label[w] < min_lbl)
                        min_lbl = label[w];
                }
                nbrs.push_back(std::make_pair(min_lbl, u));
            }
        }
        std::sort(nbrs.begin(), nbrs.end());
        for (std::size_t i = 0; i < nbrs.size(); ++i) {
            int u = nbrs[i].second;
            if (label[u] == -1) {
                label[u] = (int)order.size();
                order.push_back(u);
            }
        }
    }

    std::string code;
    code.reserve(n * (n - 1) / 2);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            int u = order[i], v = order[j];
            bool is_adj = false;
            for (std::size_t k = 0; k < adj_list[u].size(); ++k) {
                if (adj_list[u][k] == v) { is_adj = true; break; }
            }
            code += (is_adj ? '1' : '0');
        }
    }
    return code;
}

inline std::string fullerene_canonical_form(
    int n,
    const std::vector<std::vector<int> >& adj_list) {
    std::string min_code;
    bool first = true;
    for (int s = 1; s <= n; ++s) {
        std::string code = fullerene_bfs_code(n, adj_list, s);
        if (first || code < min_code) {
            min_code = code;
            first = false;
        }
    }
    return min_code;
}

// ---- Reverse search for triangulations (choose pattern) ----

inline void triang_enum_choose(TriangEnumState& state,
                                const std::vector<int>& available,
                                std::size_t start,
                                int chosen_count,
                                int min_size, int max_size,
                                int fullerene_n,
                                std::set<std::string>* seen,
                                std::vector<FullereneEnumeratedGraph>* out);

inline void triang_enum_dfs(TriangEnumState& state,
                             int fullerene_n,
                             std::set<std::string>* seen,
                             std::vector<FullereneEnumeratedGraph>* out) {
    int v = state.total_v;

    if (state.alive_count == v) {
        // Final check: all degrees are 5 or 6, edge count = 3v - 6
        int edge_count = 0;
        for (int u = 1; u <= v; ++u) {
            if (state.deg[u] < 5 || state.deg[u] > 6) return;
            edge_count += state.deg[u];
        }
        edge_count /= 2;
        if (edge_count != 3 * v - 6) return;

        // Check that there are exactly 12 vertices of degree 5
        int deg5_count = 0;
        for (int u = 1; u <= v; ++u) {
            if (state.deg[u] == 5) deg5_count++;
        }
        if (deg5_count != 12) return;

        // Planarity check
        {
            std::vector<std::pair<int, int> > edges;
            for (int u = 1; u <= v; ++u)
                for (int w = u + 1; w <= v; ++w)
                    if (state.adj[u][w])
                        edges.push_back(std::make_pair(u, w));
            Graph g(v, edges);
            PlanarResult pr = check_planar(g);
            if (!pr.is_planar) return;
        }

        // Extract faces
        std::vector<std::vector<int> > faces =
            extract_triangulation_faces(v, state.adj);
        if (faces.empty()) return;

        // Build dual (fullerene)
        std::vector<std::pair<int, int> > fullerene_edges;
        if (!build_dual_fullerene(v, state.adj, faces,
                                   fullerene_n, fullerene_edges))
            return;

        // Build adjacency list
        std::vector<std::vector<int> > adj_list(fullerene_n + 1);
        for (std::size_t i = 0; i < fullerene_edges.size(); ++i) {
            adj_list[fullerene_edges[i].first].push_back(
                fullerene_edges[i].second);
            adj_list[fullerene_edges[i].second].push_back(
                fullerene_edges[i].first);
        }
        for (int u = 1; u <= fullerene_n; ++u)
            std::sort(adj_list[u].begin(), adj_list[u].end());

        // Check duplicates using canonical form
        std::string canon =
            fullerene_canonical_form(fullerene_n, adj_list);
        if (seen->count(canon)) return;
        seen->insert(canon);

        FullereneEnumeratedGraph graph;
        graph.n = fullerene_n;
        graph.edges = fullerene_edges;
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int remaining = state.total_v - x;

    // Available vertices with deg < 6
    std::vector<int> available;
    for (int u = 1; u < x; ++u) {
        if (state.deg[u] < 6) available.push_back(u);
    }

    // Min/max number of neighbors for the new vertex
    int min_neighbors = 5 - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    // Triangulation: the first vertex may have 0 edges
    if (x <= 3) min_neighbors = (x <= 1) ? 0 : (x - 1);
    int max_neighbors = 6;
    if (max_neighbors > (int)available.size())
        max_neighbors = (int)available.size();
    // Edge count upper bound for triangulations: limit on addable edges
    if (max_neighbors > x - 1) max_neighbors = x - 1;

    if (max_neighbors < min_neighbors) return;

    triang_enum_choose(state, available, 0, 0,
                        min_neighbors, max_neighbors,
                        fullerene_n, seen, out);
}

inline void triang_enum_choose(TriangEnumState& state,
                                const std::vector<int>& available,
                                std::size_t start,
                                int chosen_count,
                                int min_size, int max_size,
                                int fullerene_n,
                                std::set<std::string>* seen,
                                std::vector<FullereneEnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_v - x;

    if (chosen_count >= min_size) {
        state.deg[x] = chosen_count;
        state.alive_count = x;

        bool feasible = true;

        // Pruning: degree reachability (deg >= 5 required at the end)
        for (int u = 1; u <= x; ++u) {
            if (state.deg[u] + remaining_after_x < 5) {
                feasible = false;
                break;
            }
            if (state.deg[u] > 6) {
                feasible = false;
                break;
            }
        }

        // Pruning: connectivity
        if (feasible && x >= 2) {
            int comp = triang_count_components(state, x);
            if (comp > remaining_after_x + 1) feasible = false;
        }

        // Pruning: edge count upper bound (planar graph: m <= 3v - 6)
        if (feasible && x >= 3) {
            int edge_count = 0;
            for (int u = 1; u <= x; ++u) edge_count += state.deg[u];
            edge_count /= 2;
            if (edge_count > 3 * x - 6) feasible = false;
        }

        // Pruning: planarity (expensive, applied only when edge count is high)
        if (feasible && x >= 5) {
            int edge_count = 0;
            for (int u = 1; u <= x; ++u) edge_count += state.deg[u];
            edge_count /= 2;
            if (edge_count >= 3 * x - 8) {
                std::vector<std::pair<int, int> > edges;
                for (int u = 1; u <= x; ++u)
                    for (int w = u + 1; w <= x; ++w)
                        if (state.adj[u][w])
                            edges.push_back(std::make_pair(u, w));
                Graph g(x, edges);
                PlanarResult pr = check_planar(g);
                if (!pr.is_planar) feasible = false;
            }
        }

        if (feasible) {
            triang_enum_dfs(state, fullerene_n, seen, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int u = available[i];
        if (state.deg[u] >= 6) continue;

        state.adj[x][u] = 1;
        state.adj[u][x] = 1;
        state.deg[u]++;

        triang_enum_choose(state, available, i + 1, chosen_count + 1,
                            min_size, max_size,
                            fullerene_n, seen, out);

        state.adj[x][u] = 0;
        state.adj[u][x] = 0;
        state.deg[u]--;
    }
}

}  // namespace detail

/**
 * @brief Enumerates all non-isomorphic fullerene graphs on n vertices
 * @param n Number of vertices
 * @param algo Algorithm selection (currently only REVERSE_SEARCH)
 * @return FullereneEnumerationResult
 */
inline FullereneEnumerationResult
enumerate_fullerene_graphs(int n,
    FullereneEnumAlgorithm algo =
        FullereneEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    FullereneEnumerationResult result;

    if (n < 20) return result;
    if (n % 2 != 0) return result;
    if (n == 22) return result;

    // Number of vertices in the dual triangulation
    int v = n / 2 + 2;

    std::set<std::string> seen;
    detail::TriangEnumState root(v);
    detail::triang_enum_dfs(root, n, &seen, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
