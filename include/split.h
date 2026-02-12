#ifndef GRAPH_RECOGNITION_SPLIT_H
#define GRAPH_RECOGNITION_SPLIT_H

/**
 * @file split.h
 * @brief スプリットグラフ (split graph) 認識
 *
 * アルゴリズム:
 *   - DEGREE_SEQUENCE: G と補グラフの両方が弦グラフかチェック
 *   - HAMMER_SIMEONE: Hammer-Simeone 次数列条件 (デフォルト)
 */

#include "chordal.h"
#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief スプリットグラフ認識アルゴリズムの選択
 */
enum class SplitAlgorithm {
    DEGREE_SEQUENCE, /**< 補グラフ + 弦性判定 */
    HAMMER_SIMEONE   /**< Hammer-Simeone 次数列条件 (デフォルト) */
};

/**
 * @brief スプリットグラフ認識の結果
 */
struct SplitResult {
    bool is_split; /**< スプリットグラフであれば true */
};

namespace detail {

/** @brief 補グラフを構築する (内部関数) */
inline Graph build_complement_graph_split(const Graph& g) {
    std::vector<std::pair<int, int>> edges;
    edges.reserve((size_t)g.n * (size_t)(g.n - 1) / 2);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            if (g.has_edge(u, v)) continue;
            edges.push_back(std::make_pair(u, v));
        }
    }
    return Graph(g.n, edges);
}

/** @brief G と補グラフの弦性によるスプリットグラフ認識 (元のアルゴリズム) */
inline SplitResult check_split_complement(const Graph& g) {
    SplitResult res;
    res.is_split = false;

    ChordalResult g_chordal = check_chordal(g);
    if (!g_chordal.is_chordal) return res;

    Graph gc = build_complement_graph_split(g);
    ChordalResult gc_chordal = check_chordal(gc);
    if (!gc_chordal.is_chordal) return res;

    res.is_split = true;
    return res;
}

/**
 * @brief Hammer-Simeone 次数列条件によるスプリットグラフ認識
 *
 * 次数列 d1 >= d2 >= ... >= dn に対し、
 * m = max{i : di >= i-1} として
 * Σ_{i=1}^{m} di = m(m-1) + Σ_{i=m+1}^{n} di
 * が成立すればスプリットグラフ。
 */
inline SplitResult check_split_hammer_simeone(const Graph& g) {
    SplitResult res;
    res.is_split = false;

    int n = g.n;
    if (n == 0) { res.is_split = true; return res; }

    // 次数計算
    std::vector<int> deg(n);
    for (int v = 1; v <= n; ++v) {
        deg[v - 1] = (int)g.adj[v].size();
    }

    // カウンティングソート (降順)
    std::vector<int> cnt(n, 0);
    for (int i = 0; i < n; ++i) cnt[deg[i]]++;
    std::vector<int> d(n);
    int pos = 0;
    for (int k = n - 1; k >= 0; --k) {
        for (int c = 0; c < cnt[k]; ++c) {
            d[pos++] = k;
        }
    }

    // m_val = max{i : d[i-1] >= i-1} (1-indexed)
    int m_val = 0;
    for (int i = 1; i <= n; ++i) {
        if (d[i - 1] >= i - 1) {
            m_val = i;
        } else {
            break;
        }
    }

    // 条件チェック: Σ_{i=1}^{m} d[i] = m(m-1) + Σ_{i=m+1}^{n} d[i]
    long long left_sum = 0;
    for (int i = 0; i < m_val; ++i) left_sum += d[i];

    long long right_sum = 0;
    for (int i = m_val; i < n; ++i) right_sum += d[i];

    long long target = (long long)m_val * (m_val - 1) + right_sum;

    if (left_sum == target) {
        res.is_split = true;
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフがスプリットグラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: HAMMER_SIMEONE)
 * @return SplitResult
 */
inline SplitResult check_split(const Graph& g,
    SplitAlgorithm algo = SplitAlgorithm::HAMMER_SIMEONE) {
    switch (algo) {
        case SplitAlgorithm::DEGREE_SEQUENCE:
            return detail::check_split_complement(g);
        case SplitAlgorithm::HAMMER_SIMEONE:
            return detail::check_split_hammer_simeone(g);
    }
    return SplitResult();
}

} // namespace graph_recognition

#endif
