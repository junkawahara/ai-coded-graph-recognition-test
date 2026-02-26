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
 *   - C1P: 分割細分化 (partition refinement) による consecutive ones property
 *          判定 O(n+m) (デフォルト)
 */

#include "bipartite.h"
#include "graph.h"
#include <algorithm>
#include <list>
#include <vector>

namespace graph_recognition {

/**
 * @brief 凸二部グラフ認識アルゴリズムの選択
 */
enum class ConvexBipartiteAlgorithm {
    BRUTE_FORCE, /**< 全順列 C1P チェック */
    C1P          /**< 分割細分化による C1P 判定 O(n+m) (デフォルト) */
};

/**
 * @brief 凸二部グラフ認識の結果
 */
struct ConvexBipartiteResult {
    bool is_convex_bipartite; /**< 凸二部グラフであれば true */
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

// ---------------------------------------------------------------
// 分割細分化 (Partition Refinement) による C1P 判定
//
// 列の集合を「パート」のリストとして管理する。
// 各行 S を処理するとき、各パートを S に含まれる列と
// S に含まれない列に分割する。
// C1P が成立するなら、最終的に各行の 1 は連続するパート群に
// 対応する。分割後、S に含まれる部分が連続しているかを検証する。
// ---------------------------------------------------------------

/**
 * @brief 分割細分化による C1P 判定
 *
 * @param rows 各行の 1 の列インデックス集合 (0-indexed)
 * @param num_cols 列数
 * @param out_perm 成功時に列の順列を格納
 * @return C1P なら true
 */
inline bool check_c1p_partition_refinement(
    const std::vector<std::vector<int>>& rows,
    int num_cols,
    std::vector<int>& out_perm) {

    if (num_cols == 0) {
        out_perm.clear();
        return true;
    }
    if (rows.empty()) {
        out_perm.resize(num_cols);
        for (int i = 0; i < num_cols; ++i) out_perm[i] = i;
        return true;
    }

    // パートのリスト: 各パートは列インデックスの集合
    // std::list を使いパートの挿入・分割を効率的に行う
    typedef std::list<std::vector<int>> PartList;
    PartList parts;

    // 初期状態: 全列が1つのパート
    {
        std::vector<int> all(num_cols);
        for (int i = 0; i < num_cols; ++i) all[i] = i;
        parts.push_back(all);
    }

    // 各列がどのパートに属するかのマップ (part iterator)
    // → パフォーマンスのため、column -> part 内の位置を直接管理
    // 列が属するパートを示すマーカー
    std::vector<int> col_marker(num_cols, 0);

    // 行を次数の降順でソート (大きい行を先に処理)
    std::vector<int> row_order(rows.size());
    for (size_t i = 0; i < rows.size(); ++i) row_order[i] = (int)i;
    std::sort(row_order.begin(), row_order.end(),
              [&rows](int a, int b) { return rows[a].size() > rows[b].size(); });

    for (size_t ri = 0; ri < row_order.size(); ++ri) {
        const std::vector<int>& row = rows[row_order[ri]];
        if (row.empty()) continue;
        if ((int)row.size() == num_cols) continue; // 全列 → 分割不要

        // row に含まれる列をマーク
        for (size_t j = 0; j < row.size(); ++j) {
            col_marker[row[j]] = 1;
        }

        // 各パートを「row に含まれる列」と「含まれない列」に分割
        // 分割後、row に含まれるパートが連続していることを確認
        PartList new_parts;
        bool in_ones = false;     // 現在 "ones" 区間にいるか
        bool ones_ended = false;  // "ones" 区間が終わったか

        for (PartList::iterator it = parts.begin(); it != parts.end(); ++it) {
            std::vector<int> ones_part, zeros_part;
            for (size_t j = 0; j < it->size(); ++j) {
                if (col_marker[(*it)[j]]) {
                    ones_part.push_back((*it)[j]);
                } else {
                    zeros_part.push_back((*it)[j]);
                }
            }

            if (ones_part.empty()) {
                // このパートは全部 0
                if (in_ones) {
                    // ones 区間が終了
                    in_ones = false;
                    ones_ended = true;
                }
                new_parts.push_back(*it);
            } else if (zeros_part.empty()) {
                // このパートは全部 1
                if (ones_ended) {
                    // ones 区間が既に終了しているのにまた 1 が出現 → C1P 違反
                    // マーカーをクリアして失敗
                    for (size_t j = 0; j < row.size(); ++j) {
                        col_marker[row[j]] = 0;
                    }
                    return false;
                }
                in_ones = true;
                new_parts.push_back(*it);
            } else {
                // 混在: 分割が必要
                if (ones_ended) {
                    // ones 区間が既に終了しているのに 1 が出現 → C1P 違反
                    for (size_t j = 0; j < row.size(); ++j) {
                        col_marker[row[j]] = 0;
                    }
                    return false;
                }
                if (!in_ones) {
                    // まだ ones 区間に入っていない: zeros を先に、ones を後に
                    new_parts.push_back(zeros_part);
                    new_parts.push_back(ones_part);
                    in_ones = true;
                } else {
                    // ones 区間中: ones を先に、zeros を後に (ones が終わる)
                    new_parts.push_back(ones_part);
                    new_parts.push_back(zeros_part);
                    in_ones = false;
                    ones_ended = true;
                }
            }
        }

        parts = new_parts;

        // マーカーをクリア
        for (size_t j = 0; j < row.size(); ++j) {
            col_marker[row[j]] = 0;
        }
    }

    // パートリストから順列を構築
    out_perm.clear();
    out_perm.reserve(num_cols);
    for (PartList::iterator it = parts.begin(); it != parts.end(); ++it) {
        for (size_t j = 0; j < it->size(); ++j) {
            out_perm.push_back((*it)[j]);
        }
    }

    return true;
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
 * @brief 分割細分化による凸二部グラフ認識
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
        if (check_c1p_partition_refinement(rows, num_y, perm)) {
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
        if (check_c1p_partition_refinement(rows, num_x, perm)) {
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
 * 分割細分化で consecutive ones property を判定する。
 */
inline ConvexBipartiteResult check_convex_bipartite(const Graph& g,
    ConvexBipartiteAlgorithm algo = ConvexBipartiteAlgorithm::C1P) {
    switch (algo) {
        case ConvexBipartiteAlgorithm::BRUTE_FORCE:
            return detail::check_convex_bipartite_brute(g);
        case ConvexBipartiteAlgorithm::C1P:
            return detail::check_convex_bipartite_c1p(g);
    }
    return ConvexBipartiteResult();
}

} // namespace graph_recognition

#endif
