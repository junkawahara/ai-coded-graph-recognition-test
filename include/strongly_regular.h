#ifndef GRAPH_RECOGNITION_STRONGLY_REGULAR_H
#define GRAPH_RECOGNITION_STRONGLY_REGULAR_H

/**
 * @file strongly_regular.h
 * @brief 強正則グラフ (strongly regular graph) 認識
 *
 * 正則グラフで、隣接頂点対の共通隣接数 λ と
 * 非隣接頂点対の共通隣接数 μ が一定であることを検証する。
 * 自明なケース (完全グラフ、空グラフ) は除外する。
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief 強正則グラフ認識アルゴリズムの選択
 */
enum class StronglyRegularAlgorithm {
    PARAMETER_CHECK /**< パラメータ検証 */
};

/**
 * @brief 強正則グラフ認識の結果
 */
struct StronglyRegularResult {
    bool is_strongly_regular = false; /**< 強正則グラフであれば true */
    int k = -1;      /**< 正則次数 */
    int lambda = -1;  /**< 隣接対の共通隣接数 */
    int mu = -1;      /**< 非隣接対の共通隣接数 */
};

/**
 * @brief グラフが強正則グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: PARAMETER_CHECK)
 * @return StronglyRegularResult
 *
 * 強正則グラフ srg(n, k, λ, μ):
 *   - k-正則 (0 < k < n-1)
 *   - 隣接する任意の頂点対の共通隣接数が λ
 *   - 非隣接な任意の頂点対の共通隣接数が μ
 */
inline StronglyRegularResult check_strongly_regular(const Graph& g,
    StronglyRegularAlgorithm algo = StronglyRegularAlgorithm::PARAMETER_CHECK) {
    (void)algo;
    StronglyRegularResult res;

    int n = g.n;
    if (n < 2) return res;

    /* k-正則チェック */
    int k = (int)g.adj[1].size();
    for (int v = 2; v <= n; ++v) {
        if ((int)g.adj[v].size() != k) return res;
    }

    /* 自明なケース除外: 完全グラフ (k=n-1) または空グラフ (k=0) */
    if (k == 0 || k == n - 1) return res;

    /* 全頂点対の共通隣接数を計算 */
    int lambda_val = -1, mu_val = -1;

    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            int common = 0;
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int w = g.adj[u][i];
                if (g.adj_set[v].count(w)) ++common;
            }
            if (g.has_edge(u, v)) {
                /* 隣接対 */
                if (lambda_val == -1) {
                    lambda_val = common;
                } else if (lambda_val != common) {
                    return res;
                }
            } else {
                /* 非隣接対 */
                if (mu_val == -1) {
                    mu_val = common;
                } else if (mu_val != common) {
                    return res;
                }
            }
        }
    }

    if (lambda_val == -1) lambda_val = 0;
    if (mu_val == -1) mu_val = 0;

    res.is_strongly_regular = true;
    res.k = k;
    res.lambda = lambda_val;
    res.mu = mu_val;
    return res;
}

} // namespace graph_recognition

#endif
