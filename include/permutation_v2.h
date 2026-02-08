#ifndef GRAPH_RECOGNITION_PERMUTATION_V2_H
#define GRAPH_RECOGNITION_PERMUTATION_V2_H

#include "permutation.h"
#include <utility>
#include <vector>

namespace graph_recognition {

namespace detail_v2 {

// Improved comparability solver: class-by-class orientation.
// Each Gamma-class of edges is independent. For each class, try one
// direction; if propagation fails, try the reverse; if both fail,
// the graph is not a comparability graph.
// No backtracking between classes. O(nm) total.
struct ComparabilitySolverV2 {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> edge;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>> dir;  // dir[u][v] in {-1,0,1}

    explicit ComparabilitySolverV2(const std::vector<std::vector<unsigned char>>& edge_matrix)
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

            // Induced P3 forcing.
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

    void undo_trail(std::vector<std::pair<int, int>>& trail, size_t checkpoint) {
        while (trail.size() > checkpoint) {
            int u = trail.back().first;
            int v = trail.back().second;
            trail.pop_back();
            dir[u][v] = 0;
            dir[v][u] = 0;
        }
    }

    std::pair<int, int> find_unoriented_edge() const {
        for (int u = 1; u <= n; ++u) {
            for (size_t i = 0; i < neighbors[u].size(); ++i) {
                int v = neighbors[u][i];
                if (u < v && dir[u][v] == 0) {
                    return std::make_pair(u, v);
                }
            }
        }
        return std::make_pair(0, 0);
    }

    bool solve() {
        std::vector<std::pair<int, int>> trail;
        trail.reserve(m);

        while (true) {
            std::pair<int, int> e = find_unoriented_edge();
            if (e.first == 0) return true; // all edges oriented

            int u = e.first, v = e.second;
            size_t checkpoint = trail.size();

            // Try u -> v.
            {
                std::vector<std::pair<int, int>> q;
                q.reserve(m);
                if (assign_arc(u, v, trail, q) && propagate(trail, q)) {
                    // This Gamma-class is oriented. Continue to next class.
                    continue;
                }
                undo_trail(trail, checkpoint);
            }

            // Try v -> u.
            {
                std::vector<std::pair<int, int>> q;
                q.reserve(m);
                if (assign_arc(v, u, trail, q) && propagate(trail, q)) {
                    continue;
                }
                undo_trail(trail, checkpoint);
            }

            // Both directions fail: not a comparability graph.
            return false;
        }
    }
};

inline bool is_comparability_graph_v2(const std::vector<std::vector<unsigned char>>& a) {
    ComparabilitySolverV2 solver(a);
    return solver.solve();
}

} // namespace detail_v2

// Check whether a graph is a permutation graph (improved version).
// Uses class-by-class comparability solver (no backtracking between classes).
inline PermutationResult check_permutation_v2(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    if (!detail_v2::is_comparability_graph_v2(a)) return res;

    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);
    if (!detail_v2::is_comparability_graph_v2(c)) return res;

    res.is_permutation = true;
    return res;
}

} // namespace graph_recognition

#endif
