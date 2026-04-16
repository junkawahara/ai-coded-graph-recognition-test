#ifndef GRAPH_RECOGNITION_PLANAR_EMBEDDING_H
#define GRAPH_RECOGNITION_PLANAR_EMBEDDING_H

/**
 * @file planar_embedding.h
 * @brief 3-連結平面グラフの平面埋め込み (Tutte の重心写像)
 *
 * 3-連結平面グラフに対し Tutte の重心写像で座標を求め、
 * 回転系 (rotation system) と面 (faces) を抽出する。
 */

#include "graph.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace graph_recognition {

/**
 * @brief 平面埋め込みの結果
 */
struct PlanarEmbeddingResult {
    bool success = false;
    std::vector<std::vector<int>> rotation; /**< rotation[v] = v の隣接頂点の巡回順 */
    std::vector<std::vector<int>> faces;    /**< faces[i] = i 番目の面の頂点列 */
};

namespace detail {

/**
 * @brief BFS で最短閉路を探す (外面用)
 * @return 最短閉路の頂点列 (空なら閉路なし)
 */
inline std::vector<int> find_shortest_cycle(const Graph& g) {
    int n = g.n;
    int best_len = n + 1;
    std::vector<int> best_cycle;

    for (int s = 1; s <= n; ++s) {
        std::vector<int> dist(n + 1, -1);
        std::vector<int> parent(n + 1, -1);
        std::vector<int> bfs;
        dist[s] = 0;
        bfs.push_back(s);

        for (size_t qi = 0; qi < bfs.size(); ++qi) {
            int v = bfs[qi];
            if (dist[v] >= best_len / 2) break;
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    parent[u] = v;
                    bfs.push_back(u);
                } else if (parent[v] != u && dist[u] >= dist[v]) {
                    int len = dist[v] + dist[u] + 1;
                    if (len < best_len) {
                        best_len = len;
                        /* 閉路を復元 */
                        std::vector<int> path1, path2;
                        for (int x = v; x != -1; x = parent[x])
                            path1.push_back(x);
                        for (int x = u; x != -1; x = parent[x])
                            path2.push_back(x);
                        /* path1: v → ... → s, path2: u → ... → s */
                        std::reverse(path1.begin(), path1.end());
                        /* cycle: s → ... → v, u → ... → (s の直前) */
                        best_cycle.clear();
                        for (size_t k = 0; k < path1.size(); ++k)
                            best_cycle.push_back(path1[k]);
                        for (size_t k = 0; k + 1 < path2.size(); ++k)
                            best_cycle.push_back(path2[k]);
                    }
                }
            }
        }
    }
    return best_cycle;
}

/**
 * @brief Gauss 消去法で Ax = b を解く
 * @param A 係数行列 (n×n)
 * @param b 右辺ベクトル
 * @param x 解ベクトル (出力)
 * @return 解が存在すれば true
 */
inline bool gauss_solve(std::vector<std::vector<double>>& A,
    std::vector<double>& b, std::vector<double>& x) {
    int n = (int)A.size();
    x.resize(n, 0.0);

    /* 前進消去 */
    for (int col = 0; col < n; ++col) {
        /* ピボット選択 */
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

    /* 後退代入 */
    for (int col = n - 1; col >= 0; --col) {
        x[col] = b[col];
        for (int j = col + 1; j < n; ++j)
            x[col] -= A[col][j] * x[j];
    }
    return true;
}

} // namespace detail

/**
 * @brief 3-連結平面グラフの平面埋め込みを計算する
 * @param g 入力グラフ (3-連結平面であること)
 * @return PlanarEmbeddingResult
 *
 * Tutte の重心写像: 外面の頂点を凸多角形に固定し、
 * 内部頂点を隣接頂点の重心に配置。
 * 得られた座標から回転系と面を抽出する。
 */
inline PlanarEmbeddingResult compute_planar_embedding(const Graph& g) {
    PlanarEmbeddingResult res;
    int n = g.n;
    if (n <= 3) {
        /* 小さいグラフ: 自明な埋め込み */
        res.success = true;
        res.rotation.resize(n + 1);
        for (int v = 1; v <= n; ++v) {
            res.rotation[v] = g.adj[v];
        }
        /* 面抽出 */
        goto extract_faces;
    }

    {
        /* 最短閉路を外面として使用 */
        std::vector<int> outer = detail::find_shortest_cycle(g);
        if (outer.empty()) return res;

        int oc_size = (int)outer.size();

        /* 外面頂点を凸多角形に配置 */
        std::vector<char> is_outer(n + 1, 0);
        std::vector<double> px(n + 1, 0.0), py(n + 1, 0.0);

        for (int i = 0; i < oc_size; ++i) {
            is_outer[outer[i]] = 1;
            double angle = 2.0 * 3.14159265358979323846 * i / oc_size;
            px[outer[i]] = std::cos(angle) * 1000.0;
            py[outer[i]] = std::sin(angle) * 1000.0;
        }

        /* 内部頂点のインデックス */
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
            /* Laplacian 系を解く: Ax = bx, Ay = by */
            std::vector<std::vector<double>> A(k, std::vector<double>(k, 0.0));
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
            std::vector<double> bx2(bx), by2(by);
            std::vector<double> sol_x, sol_y;

            if (!detail::gauss_solve(A, bx, sol_x)) return res;
            if (!detail::gauss_solve(A2, by, sol_y)) return res;

            for (int i = 0; i < k; ++i) {
                px[inner_verts[i]] = sol_x[i];
                py[inner_verts[i]] = sol_y[i];
            }
        }

        /* 座標から回転系を構築: 各頂点の隣接頂点を角度順にソート */
        res.rotation.resize(n + 1);
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
            res.rotation[v].clear();
            for (size_t i = 0; i < angle_list.size(); ++i) {
                res.rotation[v].push_back(angle_list[i].second);
            }
        }
    }

extract_faces:
    /* 回転系から面を抽出 */
    {
        /* 各頂点の隣接頂点→インデックスの逆引き */
        std::vector<std::vector<int>> next_in_rot(n + 1);
        /* next_in_rot は使わず、直接 rotation から次を求める */

        /* 各 half-edge (u→v) に対し、次の half-edge を求める:
           v の rotation で u の次の頂点 w を見つけ、(v→w) が次。 */
        /* half-edge の集合を管理 */
        struct HalfEdge {
            int from, to;
        };

        /* visited フラグ: visited[u][v] = half-edge (u→v) が面に割り当て済み */
        std::vector<std::unordered_set<int>> he_visited(n + 1);

        /* 回転系の逆引き: rot_pos[v][u] = u が rotation[v] のどの位置にあるか */
        std::vector<std::vector<int>> rot_pos(n + 1);
        for (int v = 1; v <= n; ++v) {
            int deg_v = (int)res.rotation[v].size();
            rot_pos[v].assign(n + 1, -1);
            for (int i = 0; i < deg_v; ++i) {
                rot_pos[v][res.rotation[v][i]] = i;
            }
        }

        res.faces.clear();

        for (int u = 1; u <= n; ++u) {
            for (size_t i = 0; i < res.rotation[u].size(); ++i) {
                int v = res.rotation[u][i];
                if (he_visited[u].count(v)) continue;

                /* 面を trace */
                std::vector<int> face;
                int cu = u, cv = v;
                do {
                    he_visited[cu].insert(cv);
                    face.push_back(cu);

                    /* next half-edge: (cv → w) where w follows cu in rotation[cv] */
                    int pos = rot_pos[cv][cu];
                    int deg_cv = (int)res.rotation[cv].size();
                    int next_pos = (pos + 1) % deg_cv;
                    int w = res.rotation[cv][next_pos];

                    cu = cv;
                    cv = w;
                } while (cu != u || cv != v);

                res.faces.push_back(face);
            }
        }
    }

    res.success = true;
    return res;
}

} // namespace graph_recognition

#endif
