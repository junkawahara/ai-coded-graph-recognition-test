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
 * @brief Builds a consistently oriented rotation system of a triangulation
 *
 * Requires every vertex link to be an induced cycle, i.e. the triangulation
 * has no separating triangle. This always holds for fullerene duals:
 * fullerene graphs are cyclically 5-edge-connected (Doslic 2003), so their
 * dual triangulations have no separating triangles; candidate graphs whose
 * links have chords cannot be fullerene duals and are correctly rejected.
 *
 * The direction of each link cycle is NOT free per vertex: fixing vertex 1
 * and propagating the dart-consistency constraint over a BFS determines
 * every other direction (choosing the root's direction the other way gives
 * the mirror embedding). The previous per-vertex "start from the smallest
 * neighbor" rule rejected almost every valid triangulation.
 *
 * @return rot[w] = oriented cyclic neighbor order, empty on failure
 */
inline std::vector<std::vector<int> > orient_triangulation(
    int v,
    const std::vector<std::vector<char> >& adj) {

    std::vector<std::vector<int> > empty_result;

    // Build adjacency list
    std::vector<std::vector<int> > adj_list(v + 1);
    for (int u = 1; u <= v; ++u)
        for (int w = u + 1; w <= v; ++w)
            if (adj[u][w]) {
                adj_list[u].push_back(w);
                adj_list[w].push_back(u);
            }

    // Compute the (undirected) link cycle of each vertex:
    // in a triangulation, the neighbors of w induce a cycle.
    std::vector<std::vector<int> > cyc(v + 1);

    for (int w = 1; w <= v; ++w) {
        const std::vector<int>& nbrs = adj_list[w];
        int d = (int)nbrs.size();
        if (d < 3) return empty_result;

        std::vector<std::vector<int> > nbr_adj(d);
        for (int i = 0; i < d; ++i) {
            for (int j = i + 1; j < d; ++j) {
                if (adj[nbrs[i]][nbrs[j]]) {
                    nbr_adj[i].push_back(j);
                    nbr_adj[j].push_back(i);
                }
            }
        }
        // Every link vertex must have exactly 2 link neighbors
        for (int i = 0; i < d; ++i) {
            if ((int)nbr_adj[i].size() != 2) return empty_result;
        }

        // Trace the cycle
        std::vector<int> cycle_idx;
        std::vector<char> used(d, 0);
        cycle_idx.push_back(0);
        used[0] = 1;
        int prev = -1, cur = 0;
        for (int step = 1; step < d; ++step) {
            bool found = false;
            for (std::size_t ni = 0; ni < nbr_adj[cur].size(); ++ni) {
                int nxt = nbr_adj[cur][ni];
                if (nxt != prev && !used[nxt]) {
                    cycle_idx.push_back(nxt);
                    used[nxt] = 1;
                    prev = cur;
                    cur = nxt;
                    found = true;
                    break;
                }
            }
            if (!found) return empty_result;
        }
        // The traced walk must close into a single cycle
        bool closing = false;
        for (std::size_t ni = 0; ni < nbr_adj[cycle_idx[d - 1]].size(); ++ni) {
            if (nbr_adj[cycle_idx[d - 1]][ni] == 0) closing = true;
        }
        if (!closing) return empty_result;

        cyc[w].reserve(d);
        for (int i = 0; i < d; ++i) cyc[w].push_back(nbrs[cycle_idx[i]]);
    }

    // Propagate a globally consistent direction for each link cycle.
    // Each vertex's cycle direction may NOT be chosen independently: for
    // a face (a, w, b) the dart rule requires that b following a in w's
    // rotation forces a to follow w in b's rotation. Fix vertex 1's
    // direction arbitrarily (the two choices give mirror embeddings) and
    // propagate via BFS; any conflict means no consistent orientation.
    std::vector<int> dir(v + 1, 0); // 0 = unset, +1 / -1
    std::vector<std::vector<int> > pos(v + 1);
    for (int w = 1; w <= v; ++w) {
        pos[w].assign(v + 1, -1);
        for (std::size_t i = 0; i < cyc[w].size(); ++i) pos[w][cyc[w][i]] = (int)i;
    }

    dir[1] = 1;
    std::vector<int> queue(1, 1);
    for (std::size_t qi = 0; qi < queue.size(); ++qi) {
        int w = queue[qi];
        int d = (int)cyc[w].size();
        for (int i = 0; i < d; ++i) {
            int a = cyc[w][i];
            int b = cyc[w][(i + dir[w] + d) % d]; // b follows a around w
            // Constraint: in b's rotation, a follows w.
            int db = (int)cyc[b].size();
            int pw = pos[b][w];
            if (pw < 0) return empty_result;
            int forced;
            if (cyc[b][(pw + 1) % db] == a) forced = 1;
            else if (cyc[b][(pw - 1 + db) % db] == a) forced = -1;
            else return empty_result;
            if (dir[b] == 0) {
                dir[b] = forced;
                queue.push_back(b);
            } else if (dir[b] != forced) {
                return empty_result;
            }
        }
    }
    for (int w = 1; w <= v; ++w) {
        if (dir[w] == 0) return empty_result; // disconnected
    }

    // Oriented rotation system
    std::vector<std::vector<int> > rot(v + 1);
    for (int w = 1; w <= v; ++w) {
        int d = (int)cyc[w].size();
        rot[w].reserve(d);
        for (int i = 0; i < d; ++i) {
            int idx = (dir[w] == 1) ? i : (d - i) % d;
            rot[w].push_back(cyc[w][idx]);
        }
    }
    return rot;
}

/**
 * @brief Extracts faces from an oriented rotation system; empty on failure
 *
 * All faces must be triangles and Euler's formula must hold.
 */
inline std::vector<std::vector<int> > extract_triangulation_faces(
    int v,
    const std::vector<std::vector<int> >& rot) {

    std::vector<std::vector<int> > empty_result;
    if (rot.empty()) return empty_result;

    std::vector<std::vector<int> > rot_pos(v + 1);
    for (int w = 1; w <= v; ++w) {
        rot_pos[w].assign(v + 1, -1);
        for (std::size_t i = 0; i < rot[w].size(); ++i)
            rot_pos[w][rot[w][i]] = (int)i;
    }

    // Extract faces via dart tracing: dart (u, w) -> (w, successor of u in rot[w])
    std::set<std::pair<int, int> > visited_darts;
    std::vector<std::vector<int> > faces;

    for (int u = 1; u <= v; ++u) {
        for (std::size_t ni = 0; ni < rot[u].size(); ++ni) {
            int w = rot[u][ni];
            if (visited_darts.count(std::make_pair(u, w))) continue;

            std::vector<int> face;
            int cu = u, cw = w;
            int max_steps = 3 * v;
            while (max_steps-- > 0) {
                if (visited_darts.count(std::make_pair(cu, cw))) break;
                visited_darts.insert(std::make_pair(cu, cw));
                face.push_back(cu);
                int p = rot_pos[cw][cu];
                int deg_cw = (int)rot[cw].size();
                int nxt = rot[cw][(p + 1) % deg_cw];
                cu = cw;
                cw = nxt;
            }
            if (max_steps <= 0) return empty_result;
            if ((int)face.size() != 3) return empty_result;
            faces.push_back(face);
        }
    }

    // Euler formula verification: V - E + F = 2
    int E = 0;
    for (int u = 1; u <= v; ++u) E += (int)rot[u].size();
    E /= 2;
    if (v - E + (int)faces.size() != 2) {
        return empty_result;
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

// ---- Canonical form from the (unique) embedding ----

/**
 * @brief Planar code of an oriented rotation system from a root dart
 *
 * Vertices are relabeled in BFS order; from each vertex its neighbors are
 * listed in rotation order starting at the parent (at the root: at root_w).
 * Isomorphic embeddings produce the same code for corresponding darts, so
 * minimizing over all darts (and both mirror images) yields a canonical
 * form. Sound for 3-connected planar graphs, whose embedding is unique up
 * to reflection (Whitney), unlike a raw-vertex-number BFS tie-break.
 */
inline std::string planar_code_from_dart(
    int v,
    const std::vector<std::vector<int> >& rot,
    const std::vector<std::vector<int> >& rot_pos,
    int root_u, int root_w) {

    std::vector<int> label(v + 1, -1);
    std::vector<int> anchor(v + 1, 0); // rotation start (parent vertex)
    std::vector<int> order;
    order.reserve(v);
    label[root_u] = 0;
    anchor[root_u] = root_w;
    order.push_back(root_u);

    std::string code;
    code.reserve((std::size_t)(6 * v));

    for (std::size_t qi = 0; qi < order.size(); ++qi) {
        int x = order[qi];
        int d = (int)rot[x].size();
        int p = rot_pos[x][anchor[x]];
        for (int k = 0; k < d; ++k) {
            int y = rot[x][(p + k) % d];
            if (label[y] == -1) {
                label[y] = (int)order.size();
                anchor[y] = x;
                order.push_back(y);
            }
            code += (char)(label[y] + 1);
        }
        code += '\0';
    }
    return code;
}

inline std::string triangulation_canonical_code(
    int v,
    const std::vector<std::vector<int> >& rot) {

    std::string min_code;
    bool first = true;

    for (int mirror = 0; mirror < 2; ++mirror) {
        std::vector<std::vector<int> > r(v + 1);
        for (int w = 1; w <= v; ++w) {
            r[w] = rot[w];
            if (mirror) std::reverse(r[w].begin(), r[w].end());
        }
        std::vector<std::vector<int> > rot_pos(v + 1);
        for (int w = 1; w <= v; ++w) {
            rot_pos[w].assign(v + 1, -1);
            for (std::size_t i = 0; i < r[w].size(); ++i)
                rot_pos[w][r[w][i]] = (int)i;
        }
        for (int u = 1; u <= v; ++u) {
            for (std::size_t i = 0; i < r[u].size(); ++i) {
                std::string code =
                    planar_code_from_dart(v, r, rot_pos, u, r[u][i]);
                if (first || code < min_code) {
                    min_code = code;
                    first = false;
                }
            }
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

        // Build a consistently oriented rotation system; success together
        // with the all-triangle + Euler check below certifies planarity,
        // so no separate (exponential) minor-based planarity test is needed.
        std::vector<std::vector<int> > rot = orient_triangulation(v, state.adj);
        if (rot.empty()) return;

        // Extract faces
        std::vector<std::vector<int> > faces =
            extract_triangulation_faces(v, rot);
        if (faces.empty()) return;

        // Build dual (fullerene)
        std::vector<std::pair<int, int> > fullerene_edges;
        if (!build_dual_fullerene(v, state.adj, faces,
                                   fullerene_n, fullerene_edges))
            return;

        // Check duplicates using the canonical embedding code of the
        // triangulation: two fullerenes are isomorphic iff their dual
        // triangulations are, and the embedding of a 3-connected planar
        // graph is unique up to reflection, which the code minimizes over.
        std::string canon = triangulation_canonical_code(v, rot);
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
