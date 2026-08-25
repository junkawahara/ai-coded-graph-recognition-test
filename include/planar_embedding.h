#ifndef GRAPH_RECOGNITION_PLANAR_EMBEDDING_H
#define GRAPH_RECOGNITION_PLANAR_EMBEDDING_H

/**
 * @file planar_embedding.h
 * @brief Planar embedding: Tutte barycentric mapping and Demoucron-Malgrange-Pertuiset
 *
 * Algorithms:
 *   - TUTTE_3CONNECTED: coordinates from Tutte's barycentric mapping, then the
 *     rotation system and faces read off them. Only for 3-connected planar
 *     graphs, and it also yields coordinates. (default)
 *   - DMP_GENERAL: Demoucron, Malgrange & Pertuiset (1964) for any planar
 *     graph. Embeds a cycle, then repeatedly embeds one path of one fragment
 *     into a face that can hold it, splitting that face in two.
 *
 * For TUTTE_3CONNECTED the outer face must be a face of the (unique)
 * embedding, i.e. a peripheral cycle: an induced non-separating cycle
 * (Tutte). Candidate cycles are the shortest cycles through each edge,
 * filtered to peripheral ones.
 *
 * Every produced embedding is validated before being returned, so success =
 * true implies a consistent embedding and bad inputs fail instead of
 * returning garbage. The two variants need different validations: a
 * 3-connected plane graph has simple faces of length >= 3, while a general
 * one need not -- a face runs along both sides of a bridge and revisits a cut
 * vertex -- so the general check only asks for Euler's formula per component
 * and that every half-edge lies on exactly one face.
 *
 * References:
 *   - G. Demoucron, Y. Malgrange, R. Pertuiset, "Graphes planaires:
 *     reconnaissance et construction de representations planaires
 *     topologiques", Rev. Francaise Recherche Operationnelle 8 (1964) 33-47.
 */

#include "block_cut_tree.h"
#include "components.h"
#include "graph.h"
#include "graph_utils.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief Algorithm selection for planar embedding
 */
enum class PlanarEmbeddingAlgorithm {
    TUTTE_3CONNECTED, /**< Tutte barycentric mapping; 3-connected planar only (default) */
    DMP_GENERAL       /**< Demoucron-Malgrange-Pertuiset; any planar graph */
};

/**
 * @brief Result of planar embedding
 */
struct PlanarEmbeddingResult {
    bool success = false;
    std::vector<std::vector<int>> rotation; /**< rotation[v] = cyclic order of neighbors of v */
    std::vector<std::vector<int>> faces;    /**< faces[i] = vertex sequence of face i */
};

namespace detail {

/**
 * @brief Shortest cycle through edge (eu, ev): BFS path ev -> eu in G - (eu, ev)
 * @return Cycle as vertex sequence starting at eu, empty if none exists
 */
inline std::vector<int> shortest_cycle_through_edge(const Graph& g,
    int eu, int ev) {
    int n = g.n;
    std::vector<int> dist(n + 1, -1), parent(n + 1, -1);
    std::vector<int> bfs;
    dist[ev] = 0;
    bfs.push_back(ev);
    for (size_t qi = 0; qi < bfs.size(); ++qi) {
        int v = bfs[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if ((v == eu && u == ev) || (v == ev && u == eu)) continue;
            if (dist[u] == -1) {
                dist[u] = dist[v] + 1;
                parent[u] = v;
                bfs.push_back(u);
            }
        }
    }
    std::vector<int> cyc;
    if (dist[eu] == -1) return cyc;
    for (int x = eu; x != -1; x = parent[x]) cyc.push_back(x);
    return cyc; /* eu -> ... -> ev, closed by the edge (ev, eu) */
}

/** @brief Is the cycle chordless (consecutive pairs are the only adjacencies)? */
inline bool cycle_is_induced(const Graph& g, const std::vector<int>& cyc) {
    int len = (int)cyc.size();
    for (int i = 0; i < len; ++i) {
        for (int j = i + 1; j < len; ++j) {
            bool consecutive = (j == i + 1) || (i == 0 && j == len - 1);
            bool adjacent = g.adj_set[cyc[i]].count(cyc[j]) > 0;
            if (consecutive != adjacent) return false;
        }
    }
    return true;
}

/** @brief Does G - V(cycle) stay connected (or become empty)? */
inline bool cycle_is_nonseparating(const Graph& g, const std::vector<int>& cyc) {
    int n = g.n;
    std::vector<char> removed(n + 1, 0);
    for (size_t i = 0; i < cyc.size(); ++i) removed[cyc[i]] = 1;

    int start = 0;
    int rest = 0;
    for (int v = 1; v <= n; ++v) {
        if (!removed[v]) {
            if (start == 0) start = v;
            ++rest;
        }
    }
    if (rest == 0) return true;

    std::vector<char> vis(n + 1, 0);
    std::vector<int> bfs(1, start);
    vis[start] = 1;
    int cnt = 1;
    for (size_t qi = 0; qi < bfs.size(); ++qi) {
        int v = bfs[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (removed[u] || vis[u]) continue;
            vis[u] = 1;
            ++cnt;
            bfs.push_back(u);
        }
    }
    return cnt == rest;
}

/**
 * @brief Solve Ax = b by Gaussian elimination
 * @param A Coefficient matrix (n x n)
 * @param b Right-hand side vector
 * @param x Solution vector (output)
 * @return true if a solution exists
 */
inline bool gauss_solve(std::vector<std::vector<double>>& A,
    std::vector<double>& b, std::vector<double>& x) {
    int n = (int)A.size();
    x.resize(n, 0.0);

    /* Forward elimination */
    for (int col = 0; col < n; ++col) {
        /* Pivot selection */
        int pivot = col;
        double best = std::fabs(A[col][col]);
        for (int row = col + 1; row < n; ++row) {
            if (std::fabs(A[row][col]) > best) {
                best = std::fabs(A[row][col]);
                pivot = row;
            }
        }
        if (best < 1e-12) return false;
        if (pivot != col) {
            std::swap(A[col], A[pivot]);
            std::swap(b[col], b[pivot]);
        }

        double div = A[col][col];
        for (int j = col; j < n; ++j) A[col][j] /= div;
        b[col] /= div;

        for (int row = col + 1; row < n; ++row) {
            double factor = A[row][col];
            for (int j = col; j < n; ++j)
                A[row][j] -= factor * A[col][j];
            b[row] -= factor * b[col];
        }
    }

    /* Back substitution */
    for (int col = n - 1; col >= 0; --col) {
        x[col] = b[col];
        for (int j = col + 1; j < n; ++j)
            x[col] -= A[col][j] * x[j];
    }
    return true;
}

/** @brief Trace all faces of a rotation system (each half-edge once) */
inline std::vector<std::vector<int>> faces_from_rotation(
    int n, const std::vector<std::vector<int>>& rotation) {
    std::vector<std::vector<int>> faces;

    /* Rotation reverse lookup: rot_pos[v][u] = position of u in rotation[v] */
    std::vector<std::vector<int>> rot_pos(n + 1);
    for (int v = 1; v <= n; ++v) {
        rot_pos[v].assign(n + 1, -1);
        for (size_t i = 0; i < rotation[v].size(); ++i) {
            rot_pos[v][rotation[v][i]] = (int)i;
        }
    }

    /* Visited flag: he_visited[u] contains v if half-edge (u->v) is done */
    std::vector<std::unordered_set<int>> he_visited(n + 1);

    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < rotation[u].size(); ++i) {
            int v = rotation[u][i];
            if (he_visited[u].count(v)) continue;

            /* Trace a face */
            std::vector<int> face;
            int cu = u, cv = v;
            do {
                he_visited[cu].insert(cv);
                face.push_back(cu);

                /* next half-edge: (cv -> w) where w follows cu in rotation[cv] */
                int pos = rot_pos[cv][cu];
                int deg_cv = (int)rotation[cv].size();
                int next_pos = (pos + 1) % deg_cv;
                int w = rotation[cv][next_pos];

                cu = cv;
                cv = w;
            } while (cu != u || cv != v);

            faces.push_back(face);
        }
    }
    return faces;
}

/**
 * @brief Validate an embedding of a connected graph
 *
 * Checks Euler's formula (f = m - n + 2) and that every face is a simple
 * cycle (length >= 3, no repeated vertices).
 */
inline bool embedding_is_valid(int n, long long m,
    const std::vector<std::vector<int>>& faces) {
    if ((long long)faces.size() != m - n + 2) return false;
    long long total_len = 0;
    for (size_t i = 0; i < faces.size(); ++i) {
        const std::vector<int>& f = faces[i];
        if (f.size() < 3) return false;
        total_len += (long long)f.size();
        std::vector<int> sorted_f(f);
        std::sort(sorted_f.begin(), sorted_f.end());
        for (size_t j = 1; j < sorted_f.size(); ++j) {
            if (sorted_f[j] == sorted_f[j - 1]) return false;
        }
    }
    /* Every half-edge on exactly one face */
    return total_len == 2 * m;
}

} // namespace detail

/**
 * @brief Computes the planar embedding of a 3-connected planar graph
 * @param g Input graph (must be 3-connected and planar)
 * @return PlanarEmbeddingResult
 *
 * Tutte's barycentric mapping: fix an outer FACE (peripheral cycle =
 * induced non-separating cycle) on a convex polygon and place internal
 * vertices at the barycenter of their neighbors. The resulting embedding
 * is validated (Euler's formula, simple faces); on inputs violating the
 * preconditions success = false is returned instead of a broken embedding.
 */
inline PlanarEmbeddingResult compute_planar_embedding_tutte(const Graph& g) {
    PlanarEmbeddingResult res;
    int n = g.n;

    if (n <= 3) {
        /* Small graph: trivial embedding */
        res.success = true;
        res.rotation.resize(n + 1);
        for (int v = 1; v <= n; ++v) {
            res.rotation[v] = g.adj[v];
        }
        res.faces = detail::faces_from_rotation(n, res.rotation);
        return res;
    }

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    /* Try each edge's shortest cycle as the outer face, filtered to
       peripheral cycles. In a 3-connected planar graph the faces are
       exactly the induced non-separating cycles (Tutte), so a separating
       or chorded candidate (e.g. the "equator" of a bipyramid) must be
       skipped: it would collapse the Tutte system. */
    for (int eu = 1; eu <= n; ++eu) {
        for (size_t ei = 0; ei < g.adj[eu].size(); ++ei) {
            int ev = g.adj[eu][ei];
            if (ev <= eu) continue;

            std::vector<int> outer =
                detail::shortest_cycle_through_edge(g, eu, ev);
            if ((int)outer.size() < 3) continue;
            if (!detail::cycle_is_induced(g, outer)) continue;
            if (!detail::cycle_is_nonseparating(g, outer)) continue;

            int oc_size = (int)outer.size();

            /* Place outer face vertices on a convex polygon */
            std::vector<char> is_outer(n + 1, 0);
            std::vector<double> px(n + 1, 0.0), py(n + 1, 0.0);

            for (int i = 0; i < oc_size; ++i) {
                is_outer[outer[i]] = 1;
                double angle = 2.0 * 3.14159265358979323846 * i / oc_size;
                px[outer[i]] = std::cos(angle) * 1000.0;
                py[outer[i]] = std::sin(angle) * 1000.0;
            }

            /* Index of internal vertices */
            std::vector<int> inner_verts;
            std::vector<int> inner_idx(n + 1, -1);
            for (int v = 1; v <= n; ++v) {
                if (!is_outer[v]) {
                    inner_idx[v] = (int)inner_verts.size();
                    inner_verts.push_back(v);
                }
            }
            int k = (int)inner_verts.size();

            if (k > 0) {
                /* Solve Laplacian system: Ax = bx, Ay = by */
                std::vector<std::vector<double>> A(
                    k, std::vector<double>(k, 0.0));
                std::vector<double> bx(k, 0.0), by(k, 0.0);

                for (int i = 0; i < k; ++i) {
                    int v = inner_verts[i];
                    A[i][i] = (double)g.adj[v].size();
                    for (size_t j = 0; j < g.adj[v].size(); ++j) {
                        int u = g.adj[v][j];
                        if (inner_idx[u] >= 0) {
                            A[i][inner_idx[u]] -= 1.0;
                        } else {
                            bx[i] += px[u];
                            by[i] += py[u];
                        }
                    }
                }

                std::vector<std::vector<double>> A2(A);
                std::vector<double> by2(by);
                std::vector<double> sol_x, sol_y;

                if (!detail::gauss_solve(A, bx, sol_x)) continue;
                if (!detail::gauss_solve(A2, by2, sol_y)) continue;

                for (int i = 0; i < k; ++i) {
                    px[inner_verts[i]] = sol_x[i];
                    py[inner_verts[i]] = sol_y[i];
                }
            }

            /* Build rotation system from coordinates:
               sort each vertex's neighbors by angle */
            std::vector<std::vector<int>> rotation(n + 1);
            for (int v = 1; v <= n; ++v) {
                std::vector<std::pair<double, int>> angle_list;
                for (size_t i = 0; i < g.adj[v].size(); ++i) {
                    int u = g.adj[v][i];
                    double dx = px[u] - px[v];
                    double dy = py[u] - py[v];
                    double a = std::atan2(dy, dx);
                    angle_list.push_back(std::make_pair(a, u));
                }
                std::sort(angle_list.begin(), angle_list.end());
                for (size_t i = 0; i < angle_list.size(); ++i) {
                    rotation[v].push_back(angle_list[i].second);
                }
            }

            std::vector<std::vector<int>> faces =
                detail::faces_from_rotation(n, rotation);
            if (!detail::embedding_is_valid(n, m, faces)) continue;

            res.rotation.swap(rotation);
            res.faces.swap(faces);
            res.success = true;
            return res;
        }
    }

    /* No peripheral cycle produced a valid embedding: the input is not a
       3-connected planar graph (or numerically degenerate). */
    return res;
}


namespace detail {

/**
 * @brief Validates an embedding of an arbitrary graph
 *
 * Only Euler's formula per connected component and "every half-edge on
 * exactly one face". Face simplicity is deliberately not required: a face of
 * a graph with a bridge runs along both of its sides, and a face at a cut
 * vertex passes through it more than once.
 */
inline bool embedding_is_valid_general(const Graph& g,
                                       const std::vector<std::vector<int>>& faces) {
    int n = g.n;
    ComponentsResult cc = connected_components(g);

    std::vector<long long> vertices(cc.count + 1, 0), edges(cc.count + 1, 0), face_count(cc.count + 1, 0);
    for (int v = 1; v <= n; ++v) {
        vertices[cc.comp[v]]++;
        edges[cc.comp[v]] += (long long)g.adj[v].size();
    }
    for (int c = 1; c <= cc.count; ++c) edges[c] /= 2;

    long long total_len = 0;
    for (size_t i = 0; i < faces.size(); ++i) {
        if (faces[i].empty()) return false;
        int c = cc.comp[faces[i][0]];
        for (size_t j = 0; j < faces[i].size(); ++j) {
            if (cc.comp[faces[i][j]] != c) return false;
        }
        face_count[c]++;
        total_len += (long long)faces[i].size();
    }

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (total_len != 2 * m) return false;

    for (int c = 1; c <= cc.count; ++c) {
        if (edges[c] == 0) {
            // A lone vertex traces no face; the plane around it is not a
            // half-edge cycle, so Euler cannot be checked by tracing.
            if (vertices[c] != 1 || face_count[c] != 0) return false;
            continue;
        }
        if (vertices[c] - edges[c] + face_count[c] != 2) return false;
    }
    return true;
}

/**
 * @brief Rebuilds a rotation system from a list of faces
 *
 * Tracing a face steps from p to v to q, which says that q follows p in the
 * rotation at v -- the same rule faces_from_rotation() uses in reverse. The
 * successor map at each vertex must be a single cycle, or the faces did not
 * come from an embedding.
 */
inline bool rotation_from_faces(int n, const std::vector<std::vector<int>>& faces,
                                std::vector<std::vector<int>>* rotation) {
    rotation->assign(n + 1, std::vector<int>());
    std::vector<std::vector<int>> succ(n + 1);
    for (int v = 1; v <= n; ++v) succ[v].assign(n + 1, 0);

    for (size_t i = 0; i < faces.size(); ++i) {
        const std::vector<int>& f = faces[i];
        size_t len = f.size();
        for (size_t j = 0; j < len; ++j) {
            int p = f[(j + len - 1) % len];
            int v = f[j];
            int q = f[(j + 1) % len];
            if (succ[v][p] != 0) return false;
            succ[v][p] = q;
        }
    }

    for (int v = 1; v <= n; ++v) {
        int start = 0;
        int degree = 0;
        for (int u = 1; u <= n; ++u) {
            if (succ[v][u] == 0) continue;
            ++degree;
            if (start == 0) start = u;
        }
        if (degree == 0) continue;
        std::vector<int>& rot = (*rotation)[v];
        int cur = start;
        for (int step = 0; step < degree; ++step) {
            rot.push_back(cur);
            cur = succ[v][cur];
            if (cur == 0) return false;
        }
        if (cur != start) return false;  // the successors did not form one cycle
    }
    return true;
}

/**
 * @brief Embeds a biconnected graph by the DMP method
 * @param h Biconnected graph on at least 3 vertices
 * @param faces Receives the faces as vertex cycles
 * @return false if h is not planar
 */
inline bool dmp_faces(const Graph& h, std::vector<std::vector<int>>* faces) {
    int n = h.n;
    faces->clear();
    if (n < 3) return false;

    std::vector<char> in_h(n + 1, 0);
    std::vector<std::vector<char>> edge_in(n + 1, std::vector<char>(n + 1, 0));

    // Start from any cycle. A biconnected graph on >= 3 vertices has one
    // through every edge.
    std::vector<int> cycle;
    for (int u = 1; u <= n && cycle.empty(); ++u) {
        for (size_t i = 0; i < h.adj[u].size() && cycle.empty(); ++i) {
            cycle = shortest_cycle_through_edge(h, u, h.adj[u][i]);
        }
    }
    if (cycle.size() < 3) return false;

    for (size_t i = 0; i < cycle.size(); ++i) {
        in_h[cycle[i]] = 1;
        int a = cycle[i], b = cycle[(i + 1) % cycle.size()];
        edge_in[a][b] = edge_in[b][a] = 1;
    }
    faces->push_back(cycle);
    std::vector<int> reversed(cycle.rbegin(), cycle.rend());
    faces->push_back(reversed);

    while (true) {
        // Fragments: each non-embedded edge between embedded vertices, and
        // each component outside the embedded part together with its
        // attachments.
        struct Fragment {
            std::vector<int> attachments;
            std::vector<int> outside;  // empty for a single-edge fragment
        };
        std::vector<Fragment> fragments;

        for (int u = 1; u <= n; ++u) {
            if (!in_h[u]) continue;
            for (size_t i = 0; i < h.adj[u].size(); ++i) {
                int v = h.adj[u][i];
                if (v <= u || !in_h[v] || edge_in[u][v]) continue;
                Fragment f;
                f.attachments.push_back(u);
                f.attachments.push_back(v);
                fragments.push_back(f);
            }
        }
        {
            std::vector<int> outside;
            for (int v = 1; v <= n; ++v) {
                if (!in_h[v]) outside.push_back(v);
            }
            std::vector<std::vector<int>> comps = induced_components(h, outside);
            for (size_t c = 0; c < comps.size(); ++c) {
                Fragment f;
                f.outside = comps[c];
                std::vector<char> seen(n + 1, 0);
                for (size_t i = 0; i < comps[c].size(); ++i) {
                    int x = comps[c][i];
                    for (size_t j = 0; j < h.adj[x].size(); ++j) {
                        int y = h.adj[x][j];
                        if (in_h[y] && !seen[y]) {
                            seen[y] = 1;
                            f.attachments.push_back(y);
                        }
                    }
                }
                fragments.push_back(f);
            }
        }
        if (fragments.empty()) break;

        // A fragment can only go into a face holding all of its attachments.
        int chosen = -1, chosen_face = -1;
        for (size_t i = 0; i < fragments.size(); ++i) {
            std::vector<int> admissible;
            for (size_t fi = 0; fi < faces->size(); ++fi) {
                std::vector<char> on_face(n + 1, 0);
                for (size_t j = 0; j < (*faces)[fi].size(); ++j) on_face[(*faces)[fi][j]] = 1;
                bool ok = true;
                for (size_t j = 0; j < fragments[i].attachments.size() && ok; ++j) {
                    if (!on_face[fragments[i].attachments[j]]) ok = false;
                }
                if (ok) admissible.push_back((int)fi);
            }
            if (admissible.empty()) return false;
            if (chosen < 0 || admissible.size() == 1) {
                chosen = (int)i;
                chosen_face = admissible[0];
            }
            // A fragment with a single admissible face has to go there, so
            // take it and stop looking; this is what makes the greedy correct.
            if (admissible.size() == 1) break;
        }

        // A path across the chosen fragment, between two of its attachments.
        const Fragment& fr = fragments[chosen];
        std::vector<int> path;
        if (fr.outside.empty()) {
            path.push_back(fr.attachments[0]);
            path.push_back(fr.attachments[1]);
        } else {
            std::vector<char> inside(n + 1, 0);
            for (size_t i = 0; i < fr.outside.size(); ++i) inside[fr.outside[i]] = 1;
            int a = fr.attachments[0];
            std::vector<int> parent(n + 1, 0);
            std::vector<int> queue;
            for (size_t i = 0; i < h.adj[a].size(); ++i) {
                int x = h.adj[a][i];
                if (inside[x] && !parent[x]) {
                    parent[x] = a;
                    queue.push_back(x);
                }
            }
            int end_inside = 0, end_attach = 0;
            for (size_t qi = 0; qi < queue.size() && !end_inside; ++qi) {
                int x = queue[qi];
                for (size_t i = 0; i < h.adj[x].size(); ++i) {
                    int y = h.adj[x][i];
                    if (inside[y]) {
                        if (!parent[y]) {
                            parent[y] = x;
                            queue.push_back(y);
                        }
                    } else if (in_h[y] && y != a) {
                        end_inside = x;
                        end_attach = y;
                        break;
                    }
                }
            }
            if (!end_inside) return false;
            std::vector<int> back;
            for (int x = end_inside; x != a; x = parent[x]) back.push_back(x);
            path.push_back(a);
            for (size_t i = back.size(); i-- > 0;) path.push_back(back[i]);
            path.push_back(end_attach);
        }

        // Split the chosen face along the path.
        const std::vector<int>& f = (*faces)[chosen_face];
        int a = path.front(), b = path.back();
        int ia = -1, ib = -1;
        for (size_t i = 0; i < f.size(); ++i) {
            if (f[i] == a && ia < 0) ia = (int)i;
            if (f[i] == b && ib < 0) ib = (int)i;
        }
        if (ia < 0 || ib < 0 || ia == ib) return false;

        std::vector<int> arc1, arc2;
        for (int i = ia;; i = (i + 1) % (int)f.size()) {
            arc1.push_back(f[i]);
            if (i == ib) break;
        }
        for (int i = ib;; i = (i + 1) % (int)f.size()) {
            arc2.push_back(f[i]);
            if (i == ia) break;
        }
        // The path's interior closes each arc into a face, traversed in
        // opposite directions so the two faces lie on opposite sides of it.
        std::vector<int> face1 = arc1, face2 = arc2;
        for (size_t i = path.size() - 1; i-- > 1;) face1.push_back(path[i]);
        for (size_t i = 1; i + 1 < path.size(); ++i) face2.push_back(path[i]);

        (*faces)[chosen_face] = face1;
        faces->push_back(face2);

        for (size_t i = 0; i < path.size(); ++i) {
            in_h[path[i]] = 1;
            if (i + 1 < path.size()) {
                edge_in[path[i]][path[i + 1]] = 1;
                edge_in[path[i + 1]][path[i]] = 1;
            }
        }
    }
    return true;
}

} // namespace detail

/**
 * @brief Computes a planar embedding of an arbitrary graph (DMP)
 * @param g Input graph
 * @return PlanarEmbeddingResult without coordinates
 *
 * The graph is cut into biconnected blocks, each embedded on its own, and the
 * rotations spliced together at the cut vertices -- two blocks share only that
 * vertex, so any interleaving of their rotation segments is a valid embedding.
 * Bridges and isolated vertices come out of the block decomposition as K2 and
 * K1 blocks and need no work.
 */
inline PlanarEmbeddingResult compute_planar_embedding_dmp(const Graph& g) {
    PlanarEmbeddingResult res;
    int n = g.n;
    res.rotation.assign(n + 1, std::vector<int>());
    if (n == 0) {
        res.success = true;
        return res;
    }

    BlockCutTreeResult bct = compute_block_cut_tree(g);
    for (size_t bi = 0; bi < bct.blocks.size(); ++bi) {
        const std::vector<int>& verts = bct.blocks[bi];
        if (verts.size() <= 1) continue;
        if (verts.size() == 2) {
            // A bridge: each endpoint simply gains the other.
            res.rotation[verts[0]].push_back(verts[1]);
            res.rotation[verts[1]].push_back(verts[0]);
            continue;
        }

        std::vector<int> orig;
        Graph sub = induced_subgraph(g, verts, &orig);
        std::vector<std::vector<int>> sub_faces;
        if (!detail::dmp_faces(sub, &sub_faces)) return PlanarEmbeddingResult();

        std::vector<std::vector<int>> sub_rotation;
        if (!detail::rotation_from_faces(sub.n, sub_faces, &sub_rotation)) {
            return PlanarEmbeddingResult();
        }
        for (int x = 1; x <= sub.n; ++x) {
            for (size_t i = 0; i < sub_rotation[x].size(); ++i) {
                res.rotation[orig[x]].push_back(orig[sub_rotation[x][i]]);
            }
        }
    }

    res.faces = detail::faces_from_rotation(n, res.rotation);
    if (!detail::embedding_is_valid_general(g, res.faces)) return PlanarEmbeddingResult();
    res.success = true;
    return res;
}

/**
 * @brief Computes a planar embedding
 * @param g Input graph
 * @param algo Algorithm to use (default: TUTTE_3CONNECTED)
 * @return PlanarEmbeddingResult
 */
inline PlanarEmbeddingResult compute_planar_embedding(const Graph& g,
    PlanarEmbeddingAlgorithm algo = PlanarEmbeddingAlgorithm::TUTTE_3CONNECTED) {
    switch (algo) {
        case PlanarEmbeddingAlgorithm::TUTTE_3CONNECTED:
            return compute_planar_embedding_tutte(g);
        case PlanarEmbeddingAlgorithm::DMP_GENERAL:
            return compute_planar_embedding_dmp(g);
        default:
            break;
    }
    return PlanarEmbeddingResult();
}

} // namespace graph_recognition

#endif
