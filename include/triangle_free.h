#ifndef GRAPH_RECOGNITION_TRIANGLE_FREE_H
#define GRAPH_RECOGNITION_TRIANGLE_FREE_H

/**
 * @file triangle_free.h
 * @brief Triangle-free グラフ (K3-free グラフ) 認識
 *
 * Triangle-free グラフとは、誘導部分グラフとして三角形 (K3) を含まない
 * グラフである。すなわち、互いに隣接する 3 頂点の組が存在しない。
 *
 * アルゴリズム:
 *   - BRUTE: 全 3 頂点組を列挙して K3 を探す O(n^3)
 *   - EDGE_PAIR: 各辺 (u,v) について共通隣接頂点の存在を検査 O(m*Delta)
 *               (デフォルト)
 *
 * 参考文献:
 *   - Folklore; 小さな禁止部分グラフの検出手法
 *   - Itai, Rodeh, "Finding a minimum circuit in a graph,"
 *     SIAM J. Comput. 7(4), 1978
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Triangle-free グラフ認識アルゴリズムの選択
 */
enum class TriangleFreeAlgorithm {
    BRUTE,     /**< 全 3 頂点組列挙 O(n^3) */
    EDGE_PAIR  /**< 辺ごとの共通隣接頂点検査 O(m*Delta) (デフォルト) */
};

/**
 * @brief Triangle-free グラフ認識の結果
 */
struct TriangleFreeResult {
    bool is_triangle_free = false; /**< triangle-free であれば true */
};

namespace detail {

/**
 * @brief 全 3 頂点組を列挙して三角形を検出
 *
 * 3 頂点の全組合せについて全ペアが隣接するかを検査する。
 * 計算量: O(n^3)
 */
inline TriangleFreeResult check_triangle_free_brute(const Graph& g) {
    TriangleFreeResult res;
    res.is_triangle_free = true;

    for (int a = 1; a <= g.n; ++a) {
        for (int b = a + 1; b <= g.n; ++b) {
            if (!g.has_edge(a, b)) continue;
            for (int c = b + 1; c <= g.n; ++c) {
                if (g.has_edge(a, c) && g.has_edge(b, c)) {
                    res.is_triangle_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief 辺ごとの共通隣接頂点検査による三角形検出 O(m*Delta)
 *
 * 各辺 (u,v) について、u の隣接頂点 w が v にも隣接するかを検査する。
 * 共通隣接頂点が 1 つでも見つかれば三角形が存在する。
 *
 * 計算量: O(m * Delta) ここで Delta は最大次数。
 */
inline TriangleFreeResult check_triangle_free_edge_pair(const Graph& g) {
    TriangleFreeResult res;
    res.is_triangle_free = true;

    int n = g.n;
    for (int u = 1; u <= n; ++u) {
        for (size_t ei = 0; ei < g.adj[u].size(); ++ei) {
            int v = g.adj[u][ei];
            if (v <= u) continue; // 各辺を 1 回だけ処理

            // u の隣接頂点が v にも隣接するか検査
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (w != v && g.has_edge(w, v)) {
                    res.is_triangle_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief グラフが triangle-free (K3-free) か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: EDGE_PAIR)
 * @return TriangleFreeResult
 *
 * G が triangle-free ⟺ 誘導部分グラフとして K3 を含まない。
 * 同値条件: 任意の辺 (u,v) について、u と v の共通隣接頂点が存在しない。
 */
inline TriangleFreeResult check_triangle_free(const Graph& g,
    TriangleFreeAlgorithm algo = TriangleFreeAlgorithm::EDGE_PAIR) {
    switch (algo) {
        case TriangleFreeAlgorithm::BRUTE:
            return detail::check_triangle_free_brute(g);
        case TriangleFreeAlgorithm::EDGE_PAIR:
            return detail::check_triangle_free_edge_pair(g);
        default:
            break;
    }
    return TriangleFreeResult();
}

} // namespace graph_recognition

#endif
