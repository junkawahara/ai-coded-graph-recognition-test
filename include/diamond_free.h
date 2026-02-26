#ifndef GRAPH_RECOGNITION_DIAMOND_FREE_H
#define GRAPH_RECOGNITION_DIAMOND_FREE_H

/**
 * @file diamond_free.h
 * @brief Diamond-free グラフ (K4-e free グラフ) 認識
 *
 * Diamond-free グラフとは、誘導部分グラフとして diamond (K4 から 1 辺を
 * 除いたグラフ、K4-e とも表記) を含まないグラフである。
 *
 * Diamond は 4 頂点 {a, b, c, d} で辺 ab, ac, ad, bc, bd が存在し cd が
 * 存在しないグラフ。すなわち辺を共有する 2 つの三角形が K4 を成さない
 * 場合に diamond が現れる。
 *
 * Diamond-free グラフの同値な特徴づけ:
 *   - 辺を共有する任意の 2 つの三角形が K4 を成す
 *   - 任意の辺 (u,v) について、u と v の共通隣接頂点の集合がクリークを成す
 *
 * アルゴリズム:
 *   - BRUTE: 全 4 頂点組を列挙して diamond を探す O(n^4)
 *   - EDGE_PAIR: 各辺の共通隣接頂点がクリークかを判定 O(nm) (デフォルト)
 *
 * 参考文献:
 *   - Folklore; 小さな禁止部分グラフの検出手法
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Diamond-free グラフ認識アルゴリズムの選択
 */
enum class DiamondFreeAlgorithm {
    BRUTE,     /**< 全 4 頂点組列挙 O(n^4) */
    EDGE_PAIR  /**< 辺ごとの共通隣接頂点検査 O(nm) (デフォルト) */
};

/**
 * @brief Diamond-free グラフ認識の結果
 */
struct DiamondFreeResult {
    bool is_diamond_free; /**< diamond-free であれば true */
};

namespace detail {

/**
 * @brief 全 4 頂点組を列挙して誘導 diamond を検出
 *
 * 4 頂点の全組合せについて辺数がちょうど 5 かを検査する。
 * 計算量: O(n^4)
 */
inline DiamondFreeResult check_diamond_free_brute(const Graph& g) {
    DiamondFreeResult res;
    res.is_diamond_free = true;

    for (int a = 1; a <= g.n; ++a) {
        for (int b = a + 1; b <= g.n; ++b) {
            for (int c = b + 1; c <= g.n; ++c) {
                for (int d = c + 1; d <= g.n; ++d) {
                    // 4 頂点間の辺数を数える
                    int cnt = 0;
                    if (g.has_edge(a, b)) cnt++;
                    if (g.has_edge(a, c)) cnt++;
                    if (g.has_edge(a, d)) cnt++;
                    if (g.has_edge(b, c)) cnt++;
                    if (g.has_edge(b, d)) cnt++;
                    if (g.has_edge(c, d)) cnt++;
                    if (cnt == 5) {
                        res.is_diamond_free = false;
                        return res;
                    }
                }
            }
        }
    }
    return res;
}

/**
 * @brief 辺ごとの共通隣接頂点検査による diamond 検出 O(nm)
 *
 * 各辺 (u,v) について、u と v の共通隣接頂点の集合 S を求める。
 * S 内に非隣接な頂点ペアがあれば、それらと u, v で diamond を構成する。
 * S がクリークであれば、この辺からは diamond は生じない。
 *
 * 各辺について共通隣接頂点数は O(min(deg(u), deg(v)))、
 * そのクリーク性検査は辺数カウントで O(Σ deg) = O(m)。
 * 全辺で合計 O(nm)。
 */
inline DiamondFreeResult check_diamond_free_edge_pair(const Graph& g) {
    DiamondFreeResult res;
    res.is_diamond_free = true;

    int n = g.n;
    std::vector<unsigned char> in_common(n + 1, 0);

    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; // 各辺を 1 回だけ処理

            // 共通隣接頂点の集合を求める
            std::vector<int> common;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (w != v && g.has_edge(w, v)) {
                    common.push_back(w);
                    in_common[w] = 1;
                }
            }

            if (common.size() < 2) {
                // 共通隣接頂点が 1 以下なら diamond は不可能
                for (size_t i = 0; i < common.size(); ++i) {
                    in_common[common[i]] = 0;
                }
                continue;
            }

            // 共通隣接頂点がクリークかを辺数で検査
            long long edge_count = 0;
            for (size_t i = 0; i < common.size(); ++i) {
                int w = common[i];
                for (size_t j = 0; j < g.adj[w].size(); ++j) {
                    if (in_common[g.adj[w][j]] && g.adj[w][j] > w) {
                        edge_count++;
                    }
                }
            }

            long long s = (long long)common.size();
            long long need = s * (s - 1) / 2;

            // クリーンアップ
            for (size_t i = 0; i < common.size(); ++i) {
                in_common[common[i]] = 0;
            }

            if (edge_count < need) {
                res.is_diamond_free = false;
                return res;
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief グラフが diamond-free (K4-e free) か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: EDGE_PAIR)
 * @return DiamondFreeResult
 *
 * G が diamond-free ⟺ 誘導部分グラフとして diamond (K4-e) を含まない。
 * 同値条件: 辺を共有する任意の 2 つの三角形が K4 を成す。
 */
inline DiamondFreeResult check_diamond_free(const Graph& g,
    DiamondFreeAlgorithm algo = DiamondFreeAlgorithm::EDGE_PAIR) {
    switch (algo) {
        case DiamondFreeAlgorithm::BRUTE:
            return detail::check_diamond_free_brute(g);
        case DiamondFreeAlgorithm::EDGE_PAIR:
            return detail::check_diamond_free_edge_pair(g);
    }
    return DiamondFreeResult();
}

} // namespace graph_recognition

#endif
