#ifndef GRAPH_RECOGNITION_CIRCLE_H
#define GRAPH_RECOGNITION_CIRCLE_H

/**
 * @file circle.h
 * @brief Circle グラフ認識 (DOW backtracking)
 *
 * Circle グラフは円の弦の交差グラフ。
 * G が circle グラフ ⟺ 長さ 2n の double occurrence word (DOW) が存在し、
 * 頂点 i, j が隣接 ⟺ DOW 内で i, j の出現が交互 (interleave)。
 *
 * バックトラッキングにより DOW を構築して判定する。
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "graph.h"

namespace graph_recognition {

enum class CircleAlgorithm {
    DOW_BACKTRACKING /**< DOW バックトラッキング */
};

struct CircleResult {
    bool is_circle;
    std::vector<int> dow; /**< 成功時の double occurrence word (長さ 2n) */
};

namespace detail_circle {

/**
 * @brief DOW バックトラッキングの内部状態
 */
struct DowState {
    int n;
    std::vector<std::vector<char>> adj; /**< 隣接行列 (1-indexed) */
    std::vector<int> word;              /**< DOW (0-indexed positions) */
    std::vector<int> first_pos;         /**< 各頂点の1回目の出現位置 (-1: 未配置) */
    std::vector<int> second_pos;        /**< 各頂点の2回目の出現位置 (-1: 未配置) */
    std::vector<int> placement;         /**< 0: 未配置, 1: 1回目配置済, 2: 完了 */
    std::vector<int> order;             /**< 配置順序 (次数降順) */
    bool found;

    explicit DowState(int n_)
        : n(n_), adj(n_ + 1, std::vector<char>(n_ + 1, 0)),
          word(2 * n_, -1), first_pos(n_ + 1, -1),
          second_pos(n_ + 1, -1), placement(n_ + 1, 0),
          order(), found(false) {}
};

/**
 * @brief 頂点 v の2回目配置時の制約チェック
 *
 * v の出現位置が [f, p] のとき、完全配置済み頂点 u の出現位置 [a, b] と
 * interleave するか判定し、adj[u][v] と一致するか検査。
 */
inline bool check_second_placement(const DowState& state, int v, int pos) {
    int f = state.first_pos[v];
    int lo = f < pos ? f : pos;
    int hi = f < pos ? pos : f;
    for (int u = 1; u <= state.n; ++u) {
        if (u == v || state.placement[u] != 2) continue;
        int a = state.first_pos[u];
        int b = state.second_pos[u];
        if (a > b) { int tmp = a; a = b; b = tmp; }
        // u と v が interleave ⟺ {a, b} のうちちょうど1つが (lo, hi) に含まれる
        bool a_in = (lo < a && a < hi);
        bool b_in = (lo < b && b < hi);
        bool interleave = (a_in != b_in);
        bool adjacent = (state.adj[u][v] != 0);
        if (interleave != adjacent) return false;
    }
    return true;
}

/**
 * @brief DOW 構築のバックトラッキング DFS
 */
inline void dow_dfs(DowState& state, int pos) {
    if (state.found) return;
    if (pos == 2 * state.n) {
        state.found = true;
        return;
    }

    // (A) 半配置頂点の2回目を配置 (制約が強い → 先に試す)
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 1) continue;
        if (!check_second_placement(state, v, pos)) continue;
        state.word[pos] = v;
        state.second_pos[v] = pos;
        state.placement[v] = 2;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 1;
        state.second_pos[v] = -1;
        state.word[pos] = -1;
    }

    // (B) 未配置頂点の1回目を配置
    for (size_t idx = 0; idx < state.order.size(); ++idx) {
        int v = state.order[idx];
        if (state.placement[v] != 0) continue;
        state.word[pos] = v;
        state.first_pos[v] = pos;
        state.placement[v] = 1;
        dow_dfs(state, pos + 1);
        if (state.found) return;
        state.placement[v] = 0;
        state.first_pos[v] = -1;
        state.word[pos] = -1;
    }
}

/**
 * @brief DOW バックトラッキングによる circle グラフ認識
 */
inline CircleResult check_circle_dow(const Graph& g) {
    CircleResult res;
    res.is_circle = false;
    int n = g.n;

    if (n == 0) {
        res.is_circle = true;
        return res;
    }

    DowState state(n);
    for (int u = 1; u <= n; ++u)
        for (size_t j = 0; j < g.adj[u].size(); ++j) {
            int v = g.adj[u][j];
            state.adj[u][v] = 1;
        }

    // 次数降順で配置順序を決定 (高次数 → 制約が多い → 枝刈りが効く)
    std::vector<std::pair<int, int>> deg_v;
    for (int v = 1; v <= n; ++v) {
        deg_v.push_back(std::make_pair(-(int)g.adj[v].size(), v));
    }
    std::sort(deg_v.begin(), deg_v.end());
    for (size_t i = 0; i < deg_v.size(); ++i) {
        state.order.push_back(deg_v[i].second);
    }

    // 頂点 order[0] の1回目を位置 0 に固定 (円環対称性を破る)
    int first_v = state.order[0];
    state.word[0] = first_v;
    state.first_pos[first_v] = 0;
    state.placement[first_v] = 1;

    dow_dfs(state, 1);

    if (state.found) {
        res.is_circle = true;
        res.dow = state.word;
    }
    return res;
}

}  // namespace detail_circle

/**
 * @brief Circle グラフ認識
 * @param g 入力グラフ (1-indexed)
 * @param algo アルゴリズム選択
 * @return CircleResult
 */
inline CircleResult check_circle(const Graph& g,
    CircleAlgorithm algo = CircleAlgorithm::DOW_BACKTRACKING) {
    (void)algo;
    return detail_circle::check_circle_dow(g);
}

}  // namespace graph_recognition

#endif
