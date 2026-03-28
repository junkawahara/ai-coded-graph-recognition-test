#ifndef GRAPH_RECOGNITION_CLAW_FREE_H
#define GRAPH_RECOGNITION_CLAW_FREE_H

/**
 * @file claw_free.h
 * @brief Claw-free グラフ (K_{1,3}-free グラフ) 認識
 *
 * Claw-free グラフとは、誘導部分グラフとして K_{1,3} (claw) を含まない
 * グラフである。K_{1,3} は中心頂点 c と、c に隣接する 3 頂点 a, b, d で
 * a, b, d が互いに非隣接なグラフ。
 *
 * Line graph の上位クラスであり、proper interval graph も claw-free。
 *
 * アルゴリズム:
 *   - TRIPLE_LOOP: 各頂点の近傍で 3 頂点の独立集合を探す O(n·Δ³)
 *   - EDGE_COUNT: 辺計数で近傍が完全かを判定し、非完全なら探索 O(m·Δ) (デフォルト)
 *
 * 参考文献:
 *   - Minty (1980); Sbihi (1980); Chudnovsky & Seymour (2005)
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief Claw-free グラフ認識アルゴリズムの選択
 */
enum class ClawFreeAlgorithm {
    TRIPLE_LOOP, /**< 三重ループ claw 検出 O(n·Δ³) */
    EDGE_COUNT   /**< 辺計数 claw 検出 O(m·Δ) (デフォルト) */
};

/**
 * @brief Claw-free グラフ認識の結果
 */
struct ClawFreeResult {
    bool is_claw_free = false; /**< claw-free であれば true */
};

namespace detail {

/**
 * @brief 三重ループによる誘導 claw (K_{1,3}) 検出
 *
 * 各頂点 c について、N(c) 内の 3 頂点 (a, b, d) が互いに非隣接かを調べる。
 * 計算量: O(n·Δ³) ただし Δ は最大次数。
 */
inline ClawFreeResult check_claw_free_triple(const Graph& g) {
    ClawFreeResult res;
    res.is_claw_free = true;

    for (int c = 1; c <= g.n; ++c) {
        if (g.adj[c].size() < 3) continue;

        const std::vector<int>& nbrs = g.adj[c];

        for (size_t i = 0; i < nbrs.size(); ++i) {
            for (size_t j = i + 1; j < nbrs.size(); ++j) {
                if (g.has_edge(nbrs[i], nbrs[j])) continue;
                for (size_t k = j + 1; k < nbrs.size(); ++k) {
                    if (g.has_edge(nbrs[i], nbrs[k])) continue;
                    if (g.has_edge(nbrs[j], nbrs[k])) continue;
                    res.is_claw_free = false;
                    return res;
                }
            }
        }
    }
    return res;
}

/**
 * @brief 辺計数による高速 claw 検出 O(m·Δ)
 *
 * 各頂点 c について N(c) 内の辺数をカウント。
 * d = deg(c) として辺数 == d(d-1)/2 なら N(c) は完全 → claw なし。
 * そうでなければ N(c) に非辺があるので claw を探す。
 */
inline ClawFreeResult check_claw_free_edge_count(const Graph& g) {
    ClawFreeResult res;
    res.is_claw_free = true;

    int n = g.n;
    std::vector<unsigned char> stamped(n + 1, 0);
    std::vector<unsigned char> a_adj(n + 1, 0);

    for (int c = 1; c <= n; ++c) {
        int d = (int)g.adj[c].size();
        if (d < 3) continue;

        // N(c) をスタンプ
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        // N(c) 内の辺数をカウント
        long long edge_count = 0;
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            int u = g.adj[c][i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (stamped[w] && w > u) edge_count++;
            }
        }

        // スタンプ解除
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        long long need = (long long)d * (d - 1) / 2;
        if (edge_count == need) continue; // N(c) は完全グラフ

        // N(c) に非辺あり → claw を探す
        // N(c) をスタンプ (再度)
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 1;
        }

        bool found_claw = false;
        // 非辺 (a, b) を見つけ、a にも b にも隣接しない x を探す
        for (size_t i = 0; i < g.adj[c].size() && !found_claw; ++i) {
            int a = g.adj[c][i];
            // a の N(c) 内隣接をマーク
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                if (stamped[g.adj[a][j]]) a_adj[g.adj[a][j]] = 1;
            }

            for (size_t j = i + 1; j < g.adj[c].size() && !found_claw; ++j) {
                int b = g.adj[c][j];
                if (a_adj[b]) continue; // a-b は辺

                // 非辺 (a, b) 発見。x in N(c) で x != a, b, !edge(x,a), !edge(x,b)
                for (size_t k = 0; k < g.adj[c].size(); ++k) {
                    int x = g.adj[c][k];
                    if (x == a || x == b) continue;
                    if (!a_adj[x] && !g.has_edge(x, b)) {
                        found_claw = true;
                        break;
                    }
                }
            }

            // a_adj をクリア
            for (size_t j = 0; j < g.adj[a].size(); ++j) {
                a_adj[g.adj[a][j]] = 0;
            }
        }

        // スタンプ解除
        for (size_t i = 0; i < g.adj[c].size(); ++i) {
            stamped[g.adj[c][i]] = 0;
        }

        if (found_claw) {
            res.is_claw_free = false;
            return res;
        }
    }
    return res;
}

} // namespace detail

/**
 * @brief グラフが claw-free (K_{1,3}-free) か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: EDGE_COUNT)
 * @return ClawFreeResult
 *
 * G が claw-free ⟺ 誘導部分グラフとして K_{1,3} を含まない。
 */
inline ClawFreeResult check_claw_free(const Graph& g,
    ClawFreeAlgorithm algo = ClawFreeAlgorithm::EDGE_COUNT) {
    switch (algo) {
        case ClawFreeAlgorithm::TRIPLE_LOOP:
            return detail::check_claw_free_triple(g);
        case ClawFreeAlgorithm::EDGE_COUNT:
            return detail::check_claw_free_edge_count(g);
        default:
            break;
    }
    return ClawFreeResult();
}

} // namespace graph_recognition

#endif
