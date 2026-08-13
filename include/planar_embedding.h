#ifndef GRAPH_RECOGNITION_PLANAR_EMBEDDING_H
#define GRAPH_RECOGNITION_PLANAR_EMBEDDING_H

/**
 * @file planar_embedding.h
 * @brief Planar embedding for 3-connected planar graphs (Tutte barycentric mapping)
 *
 * Computes coordinates using Tutte's barycentric mapping for 3-connected
 * planar graphs, and extracts the rotation system and faces.
 *
 * The outer face must be a face of the (unique) embedding, i.e. a
 * peripheral cycle: an induced non-separating cycle (Tutte). Candidate
 * cycles are the shortest cycles through each edge, filtered to peripheral
 * ones; every produced embedding is validated against Euler's formula and
 * face simplicity, so success = true implies a consistent embedding and
 * non-planar or non-3-connected inputs fail instead of returning garbage.
 */

#include "graph.h"

#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

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
inline PlanarEmbeddingResult compute_planar_embedding(const Graph& g) {
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

} // namespace graph_recognition

#endif
