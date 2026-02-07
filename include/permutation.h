#ifndef GRAPH_RECOGNITION_PERMUTATION_H
#define GRAPH_RECOGNITION_PERMUTATION_H

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

// Result of permutation graph recognition.
struct PermutationResult {
    bool is_permutation;
};

namespace detail {

// Backtracking transitive-orientation solver for comparability graphs.
// It orients all edges while enforcing:
//   1) transitivity: x->y and y->z implies x->z (and xz must be an edge),
//   2) no directed 2-path on non-edges (special case of transitivity).
struct ComparabilitySolver {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> edge;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>> dir;  // dir[u][v] in {-1,0,1}

    explicit ComparabilitySolver(const std::vector<std::vector<unsigned char>>& edge_matrix)
        : n((int)edge_matrix.size() - 1),
          m(0),
          edge(edge_matrix),
          neighbors(n + 1),
          dir(n + 1, std::vector<int>(n + 1, 0)) {
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                if (!edge[u][v]) continue;
                m++;
                neighbors[u].push_back(v);
                neighbors[v].push_back(u);
            }
        }
    }

    bool assign_arc(
        int u, int v,
        std::vector<std::pair<int, int>>& trail,
        std::vector<std::pair<int, int>>& q) {
        if (!edge[u][v]) return false;
        if (dir[u][v] == 1) return true;
        if (dir[u][v] == -1) return false;

        dir[u][v] = 1;
        dir[v][u] = -1;
        trail.push_back(std::make_pair(u, v));
        q.push_back(std::make_pair(u, v));
        return true;
    }

    bool propagate(
        std::vector<std::pair<int, int>>& trail,
        std::vector<std::pair<int, int>>& q) {
        size_t qi = 0;
        while (qi < q.size()) {
            int x = q[qi].first;
            int y = q[qi].second;
            qi++;

            // Induced P3 forcing around x and y.
            for (size_t i = 0; i < neighbors[x].size(); ++i) {
                int z = neighbors[x][i];
                if (z == y) continue;
                if (!edge[y][z]) {
                    if (!assign_arc(x, z, trail, q)) return false;
                }
            }
            for (size_t i = 0; i < neighbors[y].size(); ++i) {
                int z = neighbors[y][i];
                if (z == x) continue;
                if (!edge[x][z]) {
                    if (!assign_arc(z, y, trail, q)) return false;
                }
            }

            // Transitivity: predecessors of x must connect to y.
            for (size_t i = 0; i < neighbors[x].size(); ++i) {
                int p = neighbors[x][i];
                if (dir[p][x] != 1) continue;
                if (!edge[p][y]) return false;
                if (!assign_arc(p, y, trail, q)) return false;
            }

            // Transitivity: successors of y must be reachable from x.
            for (size_t i = 0; i < neighbors[y].size(); ++i) {
                int s = neighbors[y][i];
                if (dir[y][s] != 1) continue;
                if (!edge[x][s]) return false;
                if (!assign_arc(x, s, trail, q)) return false;
            }
        }
        return true;
    }

    void undo_to(
        size_t checkpoint,
        std::vector<std::pair<int, int>>& trail) {
        while (trail.size() > checkpoint) {
            int u = trail.back().first;
            int v = trail.back().second;
            trail.pop_back();
            dir[u][v] = 0;
            dir[v][u] = 0;
        }
    }

    std::pair<int, int> choose_edge() const {
        int best_u = 0, best_v = 0;
        int best_score = -1;
        for (int u = 1; u <= n; ++u) {
            for (size_t i = 0; i < neighbors[u].size(); ++i) {
                int v = neighbors[u][i];
                if (u >= v) continue;
                if (dir[u][v] != 0) continue;

                int score = 0;
                for (size_t j = 0; j < neighbors[u].size(); ++j) {
                    int z = neighbors[u][j];
                    if (z != v && !edge[v][z]) score++;
                }
                for (size_t j = 0; j < neighbors[v].size(); ++j) {
                    int z = neighbors[v][j];
                    if (z != u && !edge[u][z]) score++;
                }
                if (score > best_score) {
                    best_score = score;
                    best_u = u;
                    best_v = v;
                }
            }
        }
        return std::make_pair(best_u, best_v);
    }

    bool try_branch(
        int u, int v,
        std::vector<std::pair<int, int>>& trail) {
        size_t checkpoint = trail.size();
        std::vector<std::pair<int, int>> q;
        q.reserve(m);
        if (assign_arc(u, v, trail, q) &&
            propagate(trail, q) &&
            dfs(trail)) {
            return true;
        }
        undo_to(checkpoint, trail);
        return false;
    }

    bool dfs(std::vector<std::pair<int, int>>& trail) {
        if ((int)trail.size() == m) return true;
        std::pair<int, int> e = choose_edge();
        if (e.first == 0) return false;
        if (try_branch(e.first, e.second, trail)) return true;
        if (try_branch(e.second, e.first, trail)) return true;
        return false;
    }

    bool solve() {
        std::vector<std::pair<int, int>> trail;
        trail.reserve(m);
        return dfs(trail);
    }
};

inline std::vector<std::vector<unsigned char>> build_adj_matrix(const Graph& g) {
    std::vector<std::vector<unsigned char>> a(g.n + 1, std::vector<unsigned char>(g.n + 1, 0));
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (!g.has_edge(u, v)) continue;
            a[u][v] = 1;
            a[v][u] = 1;
        }
    }
    return a;
}

inline std::vector<std::vector<unsigned char>> build_complement_matrix(
    const std::vector<std::vector<unsigned char>>& a) {
    int n = (int)a.size() - 1;
    std::vector<std::vector<unsigned char>> c(n + 1, std::vector<unsigned char>(n + 1, 0));
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            if (a[u][v]) continue;
            c[u][v] = 1;
            c[v][u] = 1;
        }
    }
    return c;
}

inline bool is_comparability_graph(const std::vector<std::vector<unsigned char>>& a) {
    ComparabilitySolver solver(a);
    return solver.solve();
}

} // namespace detail

// Check whether a graph is a permutation graph.
// Characterization used:
//   G is a permutation graph iff both G and its complement are comparability graphs.
inline PermutationResult check_permutation(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    if (!detail::is_comparability_graph(a)) return res;

    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);
    if (!detail::is_comparability_graph(c)) return res;

    res.is_permutation = true;
    return res;
}

} // namespace graph_recognition

#endif
