#ifndef GRAPH_RECOGNITION_CONVEX_BIPARTITE_H
#define GRAPH_RECOGNITION_CONVEX_BIPARTITE_H

/**
 * @file convex_bipartite.h
 * @brief 凸二部グラフ (convex bipartite graph) 認識
 *
 * 二部グラフ G=(X,Y,E) で、Y の頂点を線形順序付けすると各 x in X の
 * 隣接頂点が連続区間になるもの。
 *
 * アルゴリズム:
 *   - BRUTE_FORCE: Y 側の全順列を試行して C1P を検査 (小グラフ向け)
 *   - C1P: PQ-tree (Booth & Lueker 1976) による consecutive ones property
 *          判定 (デフォルト)
 */

#include "bipartite.h"
#include "graph.h"
#include "pq_tree.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief 凸二部グラフ認識アルゴリズムの選択
 */
enum class ConvexBipartiteAlgorithm {
    BRUTE_FORCE, /**< 全順列 C1P チェック */
    C1P          /**< PQ-tree による C1P 判定 (デフォルト) */
};

/**
 * @brief 凸二部グラフ認識の結果
 */
struct ConvexBipartiteResult {
    bool is_convex_bipartite = false; /**< 凸二部グラフであれば true */
    std::vector<int> color;   /**< 二部彩色 (is_convex_bipartite == true の場合のみ有効) */
    std::vector<int> ordering; /**< Y 側の頂点順序 (is_convex_bipartite == true の場合のみ有効) */
};

namespace detail {

/**
 * @brief 行列が C1P (consecutive ones property) を持つかチェック (全順列)
 *
 * rows[i] は列のインデックス集合を表す。columns の全順列のうち、
 * 各行の 1 の位置が連続区間になる順列が存在するかを確認する。
 *
 * @param rows 各行の 1 の列インデックス集合 (0-indexed)
 * @param num_cols 列数
 * @param out_perm 成功時に順列を格納
 * @return C1P なら true
 */
inline bool check_c1p_brute(
    const std::vector<std::vector<int>>& rows,
    int num_cols,
    std::vector<int>& out_perm) {

    std::vector<int> perm(num_cols);
    for (int i = 0; i < num_cols; ++i) perm[i] = i;

    do {
        std::vector<int> pos(num_cols);
        for (int i = 0; i < num_cols; ++i) pos[perm[i]] = i;

        bool ok = true;
        for (size_t r = 0; r < rows.size() && ok; ++r) {
            if (rows[r].empty()) continue;
            int mn = num_cols, mx = -1;
            for (size_t j = 0; j < rows[r].size(); ++j) {
                int p = pos[rows[r][j]];
                if (p < mn) mn = p;
                if (p > mx) mx = p;
            }
            if (mx - mn + 1 != (int)rows[r].size()) ok = false;
        }
        if (ok) {
            out_perm = perm;
            return true;
        }
    } while (std::next_permutation(perm.begin(), perm.end()));

    return false;
}

/**
 * @brief ブルートフォースによる凸二部グラフ認識
 */
inline ConvexBipartiteResult check_convex_bipartite_brute(const Graph& g) {
    ConvexBipartiteResult res;
    res.is_convex_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    std::vector<int> x_verts, y_verts;
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) x_verts.push_back(v);
        else y_verts.push_back(v);
    }

    if (x_verts.empty() || y_verts.empty()) {
        res.is_convex_bipartite = true;
        res.color = bip.color;
        res.ordering = y_verts;
        return res;
    }

    // Y 側で C1P をテスト
    {
        int num_y = (int)y_verts.size();
        std::vector<int> y_id(g.n + 1, -1);
        for (int i = 0; i < num_y; ++i) y_id[y_verts[i]] = i;

        std::vector<std::vector<int>> rows;
        for (size_t i = 0; i < x_verts.size(); ++i) {
            int x = x_verts[i];
            std::vector<int> row;
            for (size_t j = 0; j < g.adj[x].size(); ++j) {
                row.push_back(y_id[g.adj[x][j]]);
            }
            if (!row.empty()) rows.push_back(row);
        }

        std::vector<int> perm;
        if (check_c1p_brute(rows, num_y, perm)) {
            res.is_convex_bipartite = true;
            res.color = bip.color;
            res.ordering.resize(num_y);
            for (int i = 0; i < num_y; ++i) {
                res.ordering[i] = y_verts[perm[i]];
            }
            return res;
        }
    }

    // X 側で C1P をテスト
    {
        int num_x = (int)x_verts.size();
        std::vector<int> x_id(g.n + 1, -1);
        for (int i = 0; i < num_x; ++i) x_id[x_verts[i]] = i;

        std::vector<std::vector<int>> rows;
        for (size_t i = 0; i < y_verts.size(); ++i) {
            int y = y_verts[i];
            std::vector<int> row;
            for (size_t j = 0; j < g.adj[y].size(); ++j) {
                row.push_back(x_id[g.adj[y][j]]);
            }
            if (!row.empty()) rows.push_back(row);
        }

        std::vector<int> perm;
        if (check_c1p_brute(rows, num_x, perm)) {
            res.is_convex_bipartite = true;
            res.color.assign(g.n + 1, -1);
            for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 1;
            for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 0;
            res.ordering.resize(num_x);
            for (int i = 0; i < num_x; ++i) {
                res.ordering[i] = x_verts[perm[i]];
            }
            return res;
        }
    }

    return res;
}

/**
 * @brief PQ-tree による凸二部グラフ認識
 */
inline ConvexBipartiteResult check_convex_bipartite_c1p(const Graph& g) {
    ConvexBipartiteResult res;
    res.is_convex_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    std::vector<int> x_verts, y_verts;
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) x_verts.push_back(v);
        else y_verts.push_back(v);
    }

    if (x_verts.empty() || y_verts.empty()) {
        res.is_convex_bipartite = true;
        res.color = bip.color;
        res.ordering = y_verts;
        return res;
    }

    // Y 側で C1P をテスト
    {
        int num_y = (int)y_verts.size();
        std::vector<int> y_id(g.n + 1, -1);
        for (int i = 0; i < num_y; ++i) y_id[y_verts[i]] = i;

        std::vector<std::vector<int>> rows;
        for (size_t i = 0; i < x_verts.size(); ++i) {
            int x = x_verts[i];
            std::vector<int> row;
            for (size_t j = 0; j < g.adj[x].size(); ++j) {
                row.push_back(y_id[g.adj[x][j]]);
            }
            if (!row.empty()) rows.push_back(row);
        }

        std::vector<int> perm;
        if (check_c1p_pq_tree(rows, num_y, perm)) {
            res.is_convex_bipartite = true;
            res.color = bip.color;
            res.ordering.resize(num_y);
            for (int i = 0; i < num_y; ++i) {
                res.ordering[i] = y_verts[perm[i]];
            }
            return res;
        }
    }

    // X 側で C1P をテスト
    {
        int num_x = (int)x_verts.size();
        std::vector<int> x_id(g.n + 1, -1);
        for (int i = 0; i < num_x; ++i) x_id[x_verts[i]] = i;

        std::vector<std::vector<int>> rows;
        for (size_t i = 0; i < y_verts.size(); ++i) {
            int y = y_verts[i];
            std::vector<int> row;
            for (size_t j = 0; j < g.adj[y].size(); ++j) {
                row.push_back(x_id[g.adj[y][j]]);
            }
            if (!row.empty()) rows.push_back(row);
        }

        std::vector<int> perm;
        if (check_c1p_pq_tree(rows, num_x, perm)) {
            res.is_convex_bipartite = true;
            res.color.assign(g.n + 1, -1);
            for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 1;
            for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 0;
            res.ordering.resize(num_x);
            for (int i = 0; i < num_x; ++i) {
                res.ordering[i] = x_verts[perm[i]];
            }
            return res;
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが凸二部グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: C1P)
 * @return ConvexBipartiteResult
 *
 * 二部グラフ G=(X,Y,E) が凸であるとは、Y (または X) の頂点を
 * 線形順序付けすると各反対側の頂点の隣接が連続区間になること。
 * PQ-tree (Booth & Lueker 1976) で consecutive ones property を判定する。
 */
inline ConvexBipartiteResult check_convex_bipartite(const Graph& g,
    ConvexBipartiteAlgorithm algo = ConvexBipartiteAlgorithm::C1P) {
    switch (algo) {
        case ConvexBipartiteAlgorithm::BRUTE_FORCE:
            return detail::check_convex_bipartite_brute(g);
        case ConvexBipartiteAlgorithm::C1P:
            return detail::check_convex_bipartite_c1p(g);
        default:
            break;
    }
    return ConvexBipartiteResult();
}

} // namespace graph_recognition

#endif
