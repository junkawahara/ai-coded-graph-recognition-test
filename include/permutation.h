#ifndef GRAPH_RECOGNITION_PERMUTATION_H
#define GRAPH_RECOGNITION_PERMUTATION_H

/**
 * @file permutation.h
 * @brief 順列グラフ (permutation graph) 認識
 *
 * G と補グラフの両方が比較可能性グラフであれば順列グラフと判定する。
 *
 * アルゴリズム:
 *   - BACKTRACKING: バックトラッキングによる推移的向き付け
 *   - CLASS_BASED: Gamma クラスごとの向き付け (O(nm), デフォルト)
 */

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 順列グラフ認識アルゴリズムの選択
 */
enum class PermutationAlgorithm {
    BACKTRACKING, /**< バックトラッキングによる推移的向き付け */
    CLASS_BASED   /**< Gamma クラスごとの向き付け (デフォルト) */
};

/**
 * @brief 順列グラフ認識の結果
 */
struct PermutationResult {
    bool is_permutation; /**< 順列グラフであれば true */
};

namespace detail {

/**
 * @brief バックトラッキングによる推移的向き付けソルバー
 */
struct ComparabilitySolver {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> edge;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>> dir;

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

            for (size_t i = 0; i < neighbors[x].size(); ++i) {
                int p = neighbors[x][i];
                if (dir[p][x] != 1) continue;
                if (!edge[p][y]) return false;
                if (!assign_arc(p, y, trail, q)) return false;
            }

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

/**
 * @brief Gamma クラスごとの推移的向き付けソルバー (改良版)
 */
struct ComparabilitySolverV2 {
    int n;
    int m;
    std::vector<std::vector<unsigned char>> edge;
    std::vector<std::vector<int>> neighbors;
    std::vector<std::vector<int>> dir;

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

            for (size_t i = 0; i < neighbors[x].size(); ++i) {
                int p = neighbors[x][i];
                if (dir[p][x] != 1) continue;
                if (!edge[p][y]) return false;
                if (!assign_arc(p, y, trail, q)) return false;
            }

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
            if (e.first == 0) return true;

            int u = e.first, v = e.second;
            size_t checkpoint = trail.size();

            {
                std::vector<std::pair<int, int>> q;
                q.reserve(m);
                if (assign_arc(u, v, trail, q) && propagate(trail, q)) {
                    continue;
                }
                undo_trail(trail, checkpoint);
            }

            {
                std::vector<std::pair<int, int>> q;
                q.reserve(m);
                if (assign_arc(v, u, trail, q) && propagate(trail, q)) {
                    continue;
                }
                undo_trail(trail, checkpoint);
            }

            return false;
        }
    }
};

/**
 * @brief グラフから隣接行列を構築する
 */
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

/**
 * @brief 隣接行列の補行列を構築する
 */
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

/**
 * @brief 隣接行列で与えられたグラフが比較可能性グラフか判定する (バックトラッキング)
 */
inline bool is_comparability_graph(const std::vector<std::vector<unsigned char>>& a) {
    ComparabilitySolver solver(a);
    return solver.solve();
}

/**
 * @brief 隣接行列で与えられたグラフが比較可能性グラフか判定する (クラスベース)
 */
inline bool is_comparability_graph_class_based(const std::vector<std::vector<unsigned char>>& a) {
    ComparabilitySolverV2 solver(a);
    return solver.solve();
}

/**
 * @brief バックトラッキングによる順列グラフ認識
 */
inline PermutationResult check_permutation_backtracking(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph(c)) return res;

    res.is_permutation = true;
    return res;
}

/**
 * @brief クラスベースによる順列グラフ認識
 */
inline PermutationResult check_permutation_class_based(const Graph& g) {
    PermutationResult res;
    res.is_permutation = false;

    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    if (!is_comparability_graph_class_based(a)) return res;

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    if (!is_comparability_graph_class_based(c)) return res;

    res.is_permutation = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフが順列グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: CLASS_BASED)
 * @return PermutationResult
 */
inline PermutationResult check_permutation(const Graph& g,
    PermutationAlgorithm algo = PermutationAlgorithm::CLASS_BASED) {
    switch (algo) {
        case PermutationAlgorithm::BACKTRACKING:
            return detail::check_permutation_backtracking(g);
        case PermutationAlgorithm::CLASS_BASED:
            return detail::check_permutation_class_based(g);
    }
    return PermutationResult();
}

} // namespace graph_recognition

#endif
