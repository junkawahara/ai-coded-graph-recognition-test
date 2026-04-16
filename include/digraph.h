#ifndef GRAPH_RECOGNITION_DIGRAPH_H
#define GRAPH_RECOGNITION_DIGRAPH_H

/**
 * @file digraph.h
 * @brief 有向グラフ (digraph) 認識
 *
 * 単純有向グラフの判定: 自己ループなし、同方向の重複弧なし。
 * 入力が有効な有向グラフであれば常に YES。
 *
 * 入力形式: n m (頂点数, 弧数) 続いて m 行の弧 u v (u→v)
 */

#include <iostream>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 有向グラフ認識アルゴリズムの選択
 */
enum class DigraphAlgorithm {
    VALIDITY_CHECK /**< 入力有効性チェック */
};

/**
 * @brief 有向グラフ認識の結果
 */
struct DigraphResult {
    bool is_digraph = false; /**< 有効な有向グラフであれば true */
};

/**
 * @brief 入力が有効な有向グラフか判定する
 * @param n 頂点数
 * @param arcs 弧リスト (u, v) = u→v
 * @param algo 使用アルゴリズム
 * @return DigraphResult
 *
 * 有向グラフ ⟺ 自己ループなし、同方向の重複弧なし。
 */
inline DigraphResult check_digraph(int n,
    const std::vector<std::pair<int, int>>& arcs,
    DigraphAlgorithm algo = DigraphAlgorithm::VALIDITY_CHECK) {
    (void)algo;
    DigraphResult res;

    if (n < 0) return res;
    if (n == 0) {
        res.is_digraph = arcs.empty();
        return res;
    }

    std::set<std::pair<int, int>> seen;
    for (size_t i = 0; i < arcs.size(); ++i) {
        int u = arcs[i].first, v = arcs[i].second;
        if (u < 1 || u > n || v < 1 || v > n) return res;
        if (u == v) return res; /* 自己ループ */
        if (!seen.insert(std::make_pair(u, v)).second) return res; /* 重複弧 */
    }

    res.is_digraph = true;
    return res;
}

} // namespace graph_recognition

#endif
