#ifndef GRAPH_RECOGNITION_TRAPEZOID_H
#define GRAPH_RECOGNITION_TRAPEZOID_H

/**
 * @file trapezoid.h
 * @brief 台形グラフ (trapezoid graph) 認識
 *
 * G が台形グラフであるための必要十分条件:
 *   1. G が余比較可能性グラフ (補グラフが推移的向き付け可能)
 *   2. 対応する半順序 P の interval dimension が 2 以下
 *
 * アルゴリズム: Cogis (1982) の 2-chain subgraph cover 特性化に基づく。
 *   idim(P) ≤ 2 ⟺ B(P) の辺集合が 2 つの chain subgraph で被覆可能
 *            ⟺ B(P) の非両立性グラフ I(B) が二部グラフ
 *
 * B(P) の構成 (二部グラフ on (L,R) = (X,X)):
 *   - 辺 (x,y) ∈ B ⟺ x ≠ y かつ NOT x <_P y
 *
 * 非両立性条件 (2+2 パターン):
 *   辺 (x₁,y₁) と (x₂,y₂) が非両立 ⟺
 *     x₁, y₁, x₂, y₂ が全て異なり, x₁ ≤_P y₂ かつ x₂ ≤_P y₁
 *   (4 元が distinct でない trivial 2K₂ は 2+2 に対応しないため除外)
 */

#include "graph.h"
#include "permutation.h"
#include <vector>
#include <queue>

namespace graph_recognition {

/**
 * @brief 台形グラフ認識アルゴリズムの選択
 */
enum class TrapezoidAlgorithm {
    CHAIN_COVER /**< 2-chain subgraph cover による interval dimension 判定 */
};

/**
 * @brief 台形グラフ認識の結果
 */
struct TrapezoidResult {
    bool is_trapezoid; /**< 台形グラフであれば true */
};

namespace detail_trapezoid {

/**
 * @brief 半順序 P の interval dimension が 2 以下か判定する
 *
 * B(P) の辺集合を列挙し、非両立性グラフ I(B) を構築して
 * 二部性を BFS で判定する。
 *
 * @param n 頂点数
 * @param comp 補グラフの隣接行列 (1-indexed)
 * @param dir 推移的向き付け行列 (dir[u][v]==1 ⟹ u <_P v)
 * @return true iff idim(P) ≤ 2
 */
inline bool check_interval_dimension_leq2(
    int n,
    const std::vector<std::vector<unsigned char>>& comp,
    const std::vector<std::vector<int>>& dir) {

    // x ≤_P y : x = y または x <_P y
    // x <_P y : comp[x][y] && dir[x][y] == 1

    // B(P) の辺を列挙: (x, y) with x ≠ y, NOT x <_P y
    struct BEdge { int x, y; };
    std::vector<BEdge> edges;

    for (int x = 1; x <= n; ++x) {
        for (int y = 1; y <= n; ++y) {
            if (x == y) continue;
            bool x_lt_y = (comp[x][y] && dir[x][y] == 1);
            if (!x_lt_y) {
                edges.push_back({x, y});
            }
        }
    }

    int m = (int)edges.size();
    if (m == 0) return true;

    // 非両立性グラフ I(B) を構築
    // 辺 (x1,y1) と (x2,y2) が非両立 ⟺ 2+2 パターン:
    //   x1, y1, x2, y2 が全て異なり, x1 <_P y2, x2 <_P y1
    std::vector<std::vector<int>> adj(m);
    for (int i = 0; i < m; ++i) {
        for (int j = i + 1; j < m; ++j) {
            int x1 = edges[i].x, y1 = edges[i].y;
            int x2 = edges[j].x, y2 = edges[j].y;
            // 4 元が全て異なることを要求
            if (x1 == x2 || x1 == y2 || y1 == x2 || y1 == y2) continue;
            // x1 <_P y2 (strict)
            bool lt1 = (comp[x1][y2] && dir[x1][y2] == 1);
            // x2 <_P y1 (strict)
            bool lt2 = (comp[x2][y1] && dir[x2][y1] == 1);
            if (lt1 && lt2) {
                adj[i].push_back(j);
                adj[j].push_back(i);
            }
        }
    }

    // BFS 二部性判定
    std::vector<int> color(m, -1);
    for (int i = 0; i < m; ++i) {
        if (color[i] != -1) continue;
        color[i] = 0;
        std::queue<int> q;
        q.push(i);
        while (!q.empty()) {
            int u = q.front(); q.pop();
            for (int v : adj[u]) {
                if (color[v] == -1) {
                    color[v] = 1 - color[u];
                    q.push(v);
                } else if (color[v] == color[u]) {
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace detail_trapezoid

/**
 * @brief グラフが台形グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: CHAIN_COVER)
 * @return TrapezoidResult
 */
inline TrapezoidResult check_trapezoid(const Graph& g,
    TrapezoidAlgorithm algo = TrapezoidAlgorithm::CHAIN_COVER) {
    (void)algo;
    TrapezoidResult res;
    res.is_trapezoid = false;

    int n = g.n;
    if (n <= 2) { res.is_trapezoid = true; return res; }

    // Step 1: 余比較可能性チェック (補グラフの推移的向き付けを取得)
    std::vector<std::vector<unsigned char>> a = detail::build_adj_matrix(g);
    std::vector<std::vector<unsigned char>> c = detail::build_complement_matrix(a);

    detail::ComparabilitySolverV2 solver(c);
    if (!solver.solve()) return res;

    // Step 2: interval dimension ≤ 2 の判定
    if (!detail_trapezoid::check_interval_dimension_leq2(g.n, c, solver.dir))
        return res;

    res.is_trapezoid = true;
    return res;
}

} // namespace graph_recognition

#endif
