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
#include <queue>
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
 * @brief 凸二部グラフ認識の共通実装
 *
 * 非連結グラフでは成分ごとに独立に X/Y の役割を交換できるため、
 * 成分ごとに両方の向きをテストして C1P が成立する向きを選択する。
 */
inline ConvexBipartiteResult check_convex_bipartite_impl(
    const Graph& g, bool use_brute) {

    ConvexBipartiteResult res;
    res.is_convex_bipartite = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    // 連結成分を BFS で求める
    std::vector<int> comp_id(g.n + 1, -1);
    int num_comps = 0;
    for (int s = 1; s <= g.n; ++s) {
        if (comp_id[s] != -1) continue;
        int c = num_comps++;
        comp_id[s] = c;
        std::queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int v = q.front(); q.pop();
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (comp_id[u] == -1) {
                    comp_id[u] = c;
                    q.push(u);
                }
            }
        }
    }

    // 各成分の頂点を二部彩色の色で分類
    std::vector<std::vector<int>> comp_a(num_comps), comp_b(num_comps);
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) comp_a[comp_id[v]].push_back(v);
        else comp_b[comp_id[v]].push_back(v);
    }

    // 成分ごとに向きを決定し、Y 側順序を構築
    std::vector<int> final_color(g.n + 1, -1);
    std::vector<int> y_ordering;

    for (int c = 0; c < num_comps; ++c) {
        std::vector<int>& a = comp_a[c];
        std::vector<int>& b = comp_b[c];

        // 辺なし成分: 任意の向き
        if (a.empty() || b.empty()) {
            for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 0;
            for (size_t i = 0; i < b.size(); ++i) {
                final_color[b[i]] = 1;
                y_ordering.push_back(b[i]);
            }
            continue;
        }

        // 向き1: a=X (行), b=Y (列) → Y側 C1P テスト
        bool found = false;
        {
            int ny = (int)b.size();
            std::vector<int> y_id(g.n + 1, -1);
            for (int i = 0; i < ny; ++i) y_id[b[i]] = i;

            std::vector<std::vector<int>> rows;
            for (size_t i = 0; i < a.size(); ++i) {
                std::vector<int> row;
                for (size_t j = 0; j < g.adj[a[i]].size(); ++j) {
                    int id = y_id[g.adj[a[i]][j]];
                    if (id >= 0) row.push_back(id);
                }
                if (!row.empty()) rows.push_back(row);
            }

            std::vector<int> perm;
            bool ok = use_brute ? check_c1p_brute(rows, ny, perm)
                                : check_c1p_pq_tree(rows, ny, perm);
            if (ok) {
                for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 0;
                for (size_t i = 0; i < b.size(); ++i) final_color[b[i]] = 1;
                for (size_t i = 0; i < perm.size(); ++i)
                    y_ordering.push_back(b[perm[i]]);
                found = true;
            }
        }

        if (found) continue;

        // 向き2: b=X (行), a=Y (列) → Y側 C1P テスト
        {
            int ny = (int)a.size();
            std::vector<int> y_id(g.n + 1, -1);
            for (int i = 0; i < ny; ++i) y_id[a[i]] = i;

            std::vector<std::vector<int>> rows;
            for (size_t i = 0; i < b.size(); ++i) {
                std::vector<int> row;
                for (size_t j = 0; j < g.adj[b[i]].size(); ++j) {
                    int id = y_id[g.adj[b[i]][j]];
                    if (id >= 0) row.push_back(id);
                }
                if (!row.empty()) rows.push_back(row);
            }

            std::vector<int> perm;
            bool ok = use_brute ? check_c1p_brute(rows, ny, perm)
                                : check_c1p_pq_tree(rows, ny, perm);
            if (ok) {
                for (size_t i = 0; i < b.size(); ++i) final_color[b[i]] = 0;
                for (size_t i = 0; i < a.size(); ++i) final_color[a[i]] = 1;
                for (size_t i = 0; i < perm.size(); ++i)
                    y_ordering.push_back(a[perm[i]]);
                found = true;
            }
        }

        if (!found) return res;
    }

    res.is_convex_bipartite = true;
    res.color = final_color;
    res.ordering = y_ordering;
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
            return detail::check_convex_bipartite_impl(g, true);
        case ConvexBipartiteAlgorithm::C1P:
            return detail::check_convex_bipartite_impl(g, false);
        default:
            break;
    }
    return ConvexBipartiteResult();
}

} // namespace graph_recognition

#endif
