#ifndef GRAPH_RECOGNITION_MEYNIEL_H
#define GRAPH_RECOGNITION_MEYNIEL_H

/**
 * @file meyniel.h
 * @brief メイニエルグラフ認識
 *
 * メイニエルグラフとは、長さ 5 以上の全ての奇サイクルが少なくとも
 * 2 本の弦を持つグラフである。
 *
 * chordal グラフの上位クラス、perfect グラフの部分クラス。
 *
 * アルゴリズム:
 *   - DIRECT_CHECK: 各辺 (u,v) について、u から v への単純パスを
 *     DFS バックトラッキングで列挙し、奇サイクル (長さ ≥ 5) で
 *     弦 ≤ 1 の obstruction を検出する。
 *
 * 参考文献:
 *   - Meyniel, "On the perfect graph conjecture," Discrete Math., 1976
 *   - Burlet, Fonlupt, "Polynomial algorithm to recognize a Meyniel
 *     graph," Annals of Discrete Math., 1984
 *   - Roussel, Rusu, "An O(m|m|) algorithm for recognizing Meyniel
 *     graphs," Discrete Math., 2001
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief メイニエルグラフ認識アルゴリズムの選択
 */
enum class MeynielAlgorithm {
    DIRECT_CHECK /**< 直接定義検査 (DFS バックトラッキング) */
};

/**
 * @brief メイニエルグラフ認識の結果
 */
struct MeynielResult {
    bool is_meyniel = false; /**< メイニエルグラフであれば true */
};

namespace detail_meyniel {

/**
 * @brief Meyniel obstruction 探索の DFS
 *
 * start_u から target_v への単純パスを探索し、
 * 偶数長 ≥ 4 のパスが見つかったとき（奇サイクル ≥ 5 を構成）、
 * サイクルの弦数が ≤ 1 であれば obstruction 発見。
 *
 * @param g グラフ
 * @param start_u 辺の始点（パスの起点）
 * @param target_v 辺の終点（パスの目標）
 * @param path 現在のパス頂点列
 * @param in_path パス上にあるかのフラグ
 * @param chord_count パス頂点間の弦の running count
 * @param depth u から cur までの辺数
 * @return obstruction が見つかれば true
 */
inline bool meyniel_obstruction_dfs(const Graph& g,
                                     int start_u, int target_v,
                                     std::vector<int>& path,
                                     std::vector<unsigned char>& in_path,
                                     int chord_count, int depth) {
    int cur = path.back();

    for (size_t i = 0; i < g.adj[cur].size(); ++i) {
        int w = g.adj[cur][i];

        if (w == target_v) {
            // 直接辺 u-v はスキップ（depth==0 のとき）
            if (depth == 0) continue;

            // サイクル長 = depth + 2 (パスの depth+1 辺 + 閉辺 v-u)
            // 奇サイクル: depth + 2 が奇数 → depth が奇数
            // 長さ ≥ 5: depth + 2 ≥ 5 → depth ≥ 3
            if (depth < 3 || depth % 2 == 0) continue;

            // サイクル: path[0]=u, path[1], ..., path[depth]=cur, v, u に戻る
            // v から path[1..depth-1] への辺が追加の弦
            // (v-path[0]=v-u は閉辺、v-path[depth]=v-cur はパス辺)
            int extra = 0;
            for (int a = 1; a < depth; ++a) {
                if (g.adj_set[target_v].count(path[a])) {
                    extra++;
                    if (chord_count + extra > 1) break;
                }
            }
            if (chord_count + extra <= 1) return true;
            continue;
        }

        if (in_path[w]) continue;

        // w をパスに追加する際の新しい弦:
        // w から path[0..depth-1] への辺（path[depth]=cur は隣接=パス辺）
        int new_chords = 0;
        for (int a = 0; a < depth; ++a) {
            if (g.adj_set[w].count(path[a])) {
                new_chords++;
            }
        }

        // 枝刈り: running count ≥ 2 なら obstruction にならない
        if (chord_count + new_chords >= 2) continue;

        in_path[w] = 1;
        path.push_back(w);
        if (meyniel_obstruction_dfs(g, start_u, target_v, path, in_path,
                                     chord_count + new_chords, depth + 1)) {
            return true;
        }
        path.pop_back();
        in_path[w] = 0;
    }
    return false;
}

}  // namespace detail_meyniel

/**
 * @brief メイニエルグラフ認識 (直接定義検査)
 *
 * 各辺 (u,v) について、u から v への単純パスを DFS で列挙。
 * 偶数長 ≥ 4 のパスが奇サイクル ≥ 5 を構成し、弦 ≤ 1 なら
 * Meyniel obstruction として検出する。
 */
inline MeynielResult check_meyniel_direct(const Graph& g) {
    MeynielResult res;
    res.is_meyniel = true;
    if (g.n <= 4) return res;

    for (int u = 1; u <= g.n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (u >= v) continue;  // 各辺を 1 回だけ検査

            std::vector<int> path;
            path.push_back(u);
            std::vector<unsigned char> in_path(g.n + 1, 0);
            in_path[u] = 1;

            if (detail_meyniel::meyniel_obstruction_dfs(
                    g, u, v, path, in_path, 0, 0)) {
                res.is_meyniel = false;
                return res;
            }
        }
    }
    return res;
}

/**
 * @brief メイニエルグラフ認識 (デフォルト)
 */
inline MeynielResult check_meyniel(
    const Graph& g,
    MeynielAlgorithm algo = MeynielAlgorithm::DIRECT_CHECK) {
    (void)algo;
    return check_meyniel_direct(g);
}

}  // namespace graph_recognition

#endif
