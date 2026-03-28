#ifndef GRAPH_RECOGNITION_BICONVEX_BIPARTITE_H
#define GRAPH_RECOGNITION_BICONVEX_BIPARTITE_H

/**
 * @file biconvex_bipartite.h
 * @brief 双凸二部グラフ (biconvex bipartite graph) 認識
 *
 * 二部グラフ G=(X,Y,E) で、X 側と Y 側の両方に線形順序を与えたとき、
 * 各頂点の反対側の隣接頂点が連続区間になるもの。
 * つまり、二部隣接行列の行と列の両方について consecutive ones property (C1P)
 * が成り立つ二部グラフ。
 *
 * アルゴリズム:
 *   - BRUTE_FORCE: 両側の全順列を試行して C1P を検査 (小グラフ向け)
 *   - C1P: PQ-tree (Booth & Lueker 1976) による C1P 判定 (デフォルト)
 *
 * 参考文献:
 *   - Abbas & Stewart, "Biconvex graphs: ordering and algorithms"
 *   - Yu & Chen
 */

#include "bipartite.h"
#include "convex_bipartite.h"
#include "graph.h"
#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 双凸二部グラフ認識アルゴリズムの選択
 */
enum class BiconvexBipartiteAlgorithm {
    BRUTE_FORCE, /**< 両側の全順列で C1P チェック */
    C1P          /**< PQ-tree による C1P 判定 (デフォルト) */
};

/**
 * @brief 双凸二部グラフ認識の結果
 */
struct BiconvexBipartiteResult {
    bool is_biconvex_bipartite; /**< 双凸二部グラフであれば true */
    std::vector<int> color;      /**< 二部彩色 (true の場合のみ有効) */
    std::vector<int> x_ordering; /**< X 側の頂点順序 (true の場合のみ有効) */
    std::vector<int> y_ordering; /**< Y 側の頂点順序 (true の場合のみ有効) */
};

namespace detail {

/**
 * @brief 二部隣接行列の片側について C1P を検査するヘルパー
 *
 * row_verts の各頂点を行とし、col_verts の頂点を列として、
 * 各行の隣接列が連続区間になるような列の順列を求める。
 *
 * @param g 入力グラフ
 * @param row_verts 行に対応する頂点集合
 * @param col_verts 列に対応する頂点集合
 * @param out_col_perm 成功時に列の順列 (col_verts のインデックス列) を格納
 * @param use_brute true なら全順列探索、false なら分割細分化
 * @return C1P なら true
 */
inline bool check_one_side_c1p(
    const Graph& g,
    const std::vector<int>& row_verts,
    const std::vector<int>& col_verts,
    std::vector<int>& out_col_perm,
    bool use_brute) {

    int num_cols = (int)col_verts.size();
    if (num_cols == 0) {
        out_col_perm.clear();
        return true;
    }

    std::vector<int> col_id(g.n + 1, -1);
    for (int i = 0; i < num_cols; ++i) col_id[col_verts[i]] = i;

    std::vector<std::vector<int>> rows;
    for (size_t i = 0; i < row_verts.size(); ++i) {
        int v = row_verts[i];
        std::vector<int> row;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int id = col_id[g.adj[v][j]];
            if (id >= 0) row.push_back(id);
        }
        if (!row.empty()) rows.push_back(row);
    }

    std::vector<int> perm;
    bool ok;
    if (use_brute) {
        ok = check_c1p_brute(rows, num_cols, perm);
    } else {
        ok = check_c1p_pq_tree(rows, num_cols, perm);
    }

    if (ok) {
        out_col_perm = perm;
    }
    return ok;
}

/**
 * @brief 双凸二部グラフ認識の共通実装
 *
 * 非連結二部グラフでは各連結成分が独立に X/Y の割当を持てるため、
 * 成分ごとに Y 側 C1P の向きを決定し、全体で合成する。
 */
inline BiconvexBipartiteResult check_biconvex_bipartite_impl(
    const Graph& g, bool use_brute) {

    BiconvexBipartiteResult res;
    res.is_biconvex_bipartite = false;

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

    // 各成分の頂点を bipartition の色で分類
    std::vector<std::vector<int>> comp_a(num_comps), comp_b(num_comps);
    for (int v = 1; v <= g.n; ++v) {
        int c = comp_id[v];
        if (bip.color[v] == 0) comp_a[c].push_back(v);
        else comp_b[c].push_back(v);
    }

    // 成分ごとに Y 側 C1P の向きを決定
    // 各成分で (A_i を行, B_i を列) または (B_i を行, A_i を列) を試す
    // 最終的な x_verts, y_verts を構築
    std::vector<int> x_verts, y_verts;
    for (int c = 0; c < num_comps; ++c) {
        std::vector<int>& a = comp_a[c];
        std::vector<int>& b = comp_b[c];

        // 辺がない成分: どちらでもよい
        if (a.empty() || b.empty()) {
            for (size_t i = 0; i < a.size(); ++i) x_verts.push_back(a[i]);
            for (size_t i = 0; i < b.size(); ++i) y_verts.push_back(b[i]);
            continue;
        }

        // 向き1: A_i を行 (X側), B_i を列 (Y側) → Y側 C1P テスト
        std::vector<int> dummy_perm;
        if (check_one_side_c1p(g, a, b, dummy_perm, use_brute)) {
            for (size_t i = 0; i < a.size(); ++i) x_verts.push_back(a[i]);
            for (size_t i = 0; i < b.size(); ++i) y_verts.push_back(b[i]);
            continue;
        }

        // 向き2: B_i を行 (X側), A_i を列 (Y側) → Y側 C1P テスト
        if (check_one_side_c1p(g, b, a, dummy_perm, use_brute)) {
            for (size_t i = 0; i < b.size(); ++i) x_verts.push_back(b[i]);
            for (size_t i = 0; i < a.size(); ++i) y_verts.push_back(a[i]);
            continue;
        }

        // どちらの向きでも Y側 C1P が成立しない → 双凸でない
        return res;
    }

    if (x_verts.empty() || y_verts.empty()) {
        res.is_biconvex_bipartite = true;
        res.color.assign(g.n + 1, -1);
        for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 0;
        for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 1;
        res.x_ordering = x_verts;
        res.y_ordering = y_verts;
        return res;
    }

    // 全体の Y 側 C1P テスト (成分ごとの向き決定後)
    std::vector<int> y_perm;
    if (!check_one_side_c1p(g, x_verts, y_verts, y_perm, use_brute)) {
        return res;
    }

    // X 側の C1P テスト (行=Y, 列=X)
    std::vector<int> x_perm;
    if (!check_one_side_c1p(g, y_verts, x_verts, x_perm, use_brute)) {
        return res;
    }

    // 両側とも C1P → 双凸二部グラフ
    res.is_biconvex_bipartite = true;
    res.color.assign(g.n + 1, -1);
    for (size_t i = 0; i < x_verts.size(); ++i) res.color[x_verts[i]] = 0;
    for (size_t i = 0; i < y_verts.size(); ++i) res.color[y_verts[i]] = 1;

    // 順列を頂点に変換
    res.x_ordering.resize(x_verts.size());
    for (size_t i = 0; i < x_perm.size(); ++i) {
        res.x_ordering[i] = x_verts[x_perm[i]];
    }
    res.y_ordering.resize(y_verts.size());
    for (size_t i = 0; i < y_perm.size(); ++i) {
        res.y_ordering[i] = y_verts[y_perm[i]];
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが双凸二部グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: C1P)
 * @return BiconvexBipartiteResult
 *
 * 二部グラフ G=(X,Y,E) が双凸であるとは、X 側と Y 側の両方に
 * 線形順序を与えたとき、各頂点の反対側の隣接が連続区間になること。
 * すなわち二部隣接行列の行と列の両方が C1P を満たす。
 */
inline BiconvexBipartiteResult check_biconvex_bipartite(const Graph& g,
    BiconvexBipartiteAlgorithm algo = BiconvexBipartiteAlgorithm::C1P) {
    switch (algo) {
        case BiconvexBipartiteAlgorithm::BRUTE_FORCE:
            return detail::check_biconvex_bipartite_impl(g, true);
        case BiconvexBipartiteAlgorithm::C1P:
            return detail::check_biconvex_bipartite_impl(g, false);
    }
    return BiconvexBipartiteResult();
}

} // namespace graph_recognition

#endif
