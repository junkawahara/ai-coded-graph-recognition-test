#ifndef GRAPH_RECOGNITION_CHAIN_H
#define GRAPH_RECOGNITION_CHAIN_H

/**
 * @file chain.h
 * @brief チェーングラフ (chain graph) 認識
 *
 * 二部グラフかつ片側の近傍がネストしていればチェーングラフと判定する。
 */

#include "bipartite.h"
#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief チェーングラフ認識アルゴリズムの選択
 */
enum class ChainAlgorithm {
    NEIGHBORHOOD_INCLUSION /**< 近傍包含判定 */
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
 * @param g 入力グラフ
 * @param side 検査する側の頂点集合
 * @param other_side 反対側の頂点集合
 * @return 近傍がネストしていれば true
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

} // namespace detail

/**
 * @brief グラフがチェーングラフか判定する
 * @param g 入力グラフ
 * @return ChainResult
 *
 * G がチェーングラフ ⟺ G が二部グラフかつ片側の近傍がネスト。
 */
inline ChainResult check_chain(const Graph& g,
    ChainAlgorithm algo = ChainAlgorithm::NEIGHBORHOOD_INCLUSION) {
    (void)algo;
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

    if (!detail::is_nested_neighborhood_side(g, left, right)) return res;

    res.is_chain = true;
    return res;
}

} // namespace graph_recognition

#endif
