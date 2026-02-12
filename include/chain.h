#ifndef GRAPH_RECOGNITION_CHAIN_H
#define GRAPH_RECOGNITION_CHAIN_H

/**
 * @file chain.h
 * @brief チェーングラフ (chain graph) 認識
 *
 * アルゴリズム:
 *   - NEIGHBORHOOD_INCLUSION: 全ペア近傍包含チェック
 *   - DEGREE_SORT: 次数ソート + 接尾辞性検証 (デフォルト)
 */

#include "bipartite.h"
#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief チェーングラフ認識アルゴリズムの選択
 */
enum class ChainAlgorithm {
    NEIGHBORHOOD_INCLUSION, /**< 全ペア近傍包含チェック */
    DEGREE_SORT             /**< 次数ソート + 接尾辞性検証 (デフォルト) */
};

/**
 * @brief チェーングラフ認識の結果
 */
struct ChainResult {
    bool is_chain; /**< チェーングラフであれば true */
};

namespace detail {

/**
 * @brief 片側の近傍が包含関係で線形順序をなすか判定する (内部関数)
 */
inline bool is_nested_neighborhood_side(
    const Graph& g,
    const std::vector<int>& side,
    const std::vector<int>& other_side) {
    for (size_t i = 0; i < side.size(); ++i) {
        int u = side[i];
        for (size_t j = i + 1; j < side.size(); ++j) {
            int v = side[j];
            bool u_subset_v = true;
            bool v_subset_u = true;
            for (size_t k = 0; k < other_side.size(); ++k) {
                int w = other_side[k];
                bool uw = g.has_edge(u, w);
                bool vw = g.has_edge(v, w);
                if (uw && !vw) u_subset_v = false;
                if (vw && !uw) v_subset_u = false;
                if (!u_subset_v && !v_subset_u) return false;
            }
        }
    }
    return true;
}

/** @brief 全ペア包含チェックによるチェーングラフ認識 (元のアルゴリズム) */
inline ChainResult check_chain_inclusion(const Graph& g) {
    ChainResult res;
    res.is_chain = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    std::vector<int> left, right;
    left.reserve(g.n);
    right.reserve(g.n);
    for (int v = 1; v <= g.n; ++v) {
        if (bip.color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    if (!is_nested_neighborhood_side(g, left, right)) return res;

    res.is_chain = true;
    return res;
}

/**
 * @brief 次数ソート + 接尾辞性検証によるチェーングラフ認識
 *
 * L 側を次数昇順にソートし、R 側各頂点の L 側隣接が
 * ソート済み L の接尾辞であることを O(n+m) で検証する。
 */
inline ChainResult check_chain_degree_sort(const Graph& g) {
    ChainResult res;
    res.is_chain = false;

    BipartiteResult bip = check_bipartite(g);
    if (!bip.is_bipartite) return res;

    int n = g.n;
    std::vector<int> left, right;
    left.reserve(n);
    right.reserve(n);
    for (int v = 1; v <= n; ++v) {
        if (bip.color[v] == 0) left.push_back(v);
        else right.push_back(v);
    }

    if (left.empty() || right.empty()) {
        res.is_chain = true;
        return res;
    }

    // L 側の各頂点の R 側次数を計算
    int left_size = (int)left.size();
    std::vector<int> deg_r(n + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            int u = g.adj[v][j];
            if (bip.color[u] == 1) deg_r[v]++;
        }
    }

    // カウンティングソートで L 側を次数昇順にソート
    int max_deg = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        if (deg_r[left[i]] > max_deg) max_deg = deg_r[left[i]];
    }
    std::vector<int> cnt(max_deg + 1, 0);
    for (size_t i = 0; i < left.size(); ++i) cnt[deg_r[left[i]]]++;

    std::vector<int> sorted_left(left_size);
    // 累積和で位置決定 (昇順)
    std::vector<int> start(max_deg + 1, 0);
    for (int k = 1; k <= max_deg; ++k) start[k] = start[k - 1] + cnt[k - 1];
    for (size_t i = 0; i < left.size(); ++i) {
        int v = left[i];
        sorted_left[start[deg_r[v]]++] = v;
    }

    // rank[v] = sorted_left 内の位置
    std::vector<int> rank_l(n + 1, -1);
    for (int i = 0; i < left_size; ++i) {
        rank_l[sorted_left[i]] = i;
    }

    // R 側各頂点について: L 側隣接の最小 rank と隣接数を検証
    for (size_t i = 0; i < right.size(); ++i) {
        int r = right[i];
        int min_rank = left_size;
        int count_l = 0;
        for (size_t j = 0; j < g.adj[r].size(); ++j) {
            int u = g.adj[r][j];
            if (bip.color[u] == 0) {
                count_l++;
                if (rank_l[u] < min_rank) min_rank = rank_l[u];
            }
        }
        if (count_l == 0) continue; // 隣接なし → OK
        // 接尾辞性: count_l == left_size - min_rank
        if (count_l != left_size - min_rank) return res;
    }

    res.is_chain = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフがチェーングラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DEGREE_SORT)
 * @return ChainResult
 */
inline ChainResult check_chain(const Graph& g,
    ChainAlgorithm algo = ChainAlgorithm::DEGREE_SORT) {
    switch (algo) {
        case ChainAlgorithm::NEIGHBORHOOD_INCLUSION:
            return detail::check_chain_inclusion(g);
        case ChainAlgorithm::DEGREE_SORT:
            return detail::check_chain_degree_sort(g);
    }
    return ChainResult();
}

} // namespace graph_recognition

#endif
