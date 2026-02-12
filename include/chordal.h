#ifndef GRAPH_RECOGNITION_CHORDAL_H
#define GRAPH_RECOGNITION_CHORDAL_H

/**
 * @file chordal.h
 * @brief 弦グラフ (chordal graph) 認識
 *
 * アルゴリズム:
 *   - MCS_PEO: 優先度キュー MCS + PEO 検証 O(n+m log n)
 *   - BUCKET_MCS_PEO: バケットソート MCS + PEO 検証 O(n+m) (デフォルト)
 */

#include "graph.h"
#include "mcs.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 弦グラフ認識アルゴリズムの選択
 */
enum class ChordalAlgorithm {
    MCS_PEO,        /**< 優先度キュー MCS + PEO 検証 O(n+m log n) */
    BUCKET_MCS_PEO  /**< バケットソート MCS + PEO 検証 O(n+m) (デフォルト) */
};

/**
 * @brief 弦グラフ認識の結果
 */
struct ChordalResult {
    bool is_chordal;                          /**< 弦グラフであれば true */
    MCSResult mcs_result;                     /**< MCS の結果 */
    std::vector<int> parent;                  /**< parent[v]: PEO における v の親 (0 なら根) */
    std::vector<std::vector<int>> later;      /**< later[v]: v より後ろの隣接頂点 */
};

namespace detail {

/**
 * @brief MCS 結果から PEO を検証し ChordalResult を構築する (共通処理)
 */
inline ChordalResult verify_peo(const Graph& g, const MCSResult& mcs_res) {
    ChordalResult res;
    int n = g.n;
    res.mcs_result = mcs_res;
    const std::vector<int>& number = res.mcs_result.number;

    res.later.resize(n + 1);
    for (int v = 1; v <= n; ++v) {
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (number[v] < number[u]) res.later[v].push_back(u);
        }
    }

    res.parent.resize(n + 1, 0);
    res.is_chordal = true;
    for (int v = 1; v <= n; ++v) {
        int best = n + 1;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (number[u] < best) {
                best = number[u];
                res.parent[v] = u;
            }
        }
        if (res.parent[v] == 0) continue;
        for (size_t i = 0; i < res.later[v].size(); ++i) {
            int u = res.later[v][i];
            if (u == res.parent[v]) continue;
            if (!g.has_edge(res.parent[v], u)) {
                res.is_chordal = false;
                return res;
            }
        }
    }

    return res;
}

} // namespace detail

/**
 * @brief グラフが弦グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: BUCKET_MCS_PEO)
 * @return ChordalResult
 *
 * MCS で PEO 候補を計算し、各頂点の later 隣接頂点がクリークを
 * なすか検証する。弦グラフなら PEO・parent・later 構造も返す。
 */
inline ChordalResult check_chordal(const Graph& g,
    ChordalAlgorithm algo = ChordalAlgorithm::BUCKET_MCS_PEO) {
    switch (algo) {
        case ChordalAlgorithm::MCS_PEO:
            return detail::verify_peo(g, mcs(g, MCSAlgorithm::PQ_MCS));
        case ChordalAlgorithm::BUCKET_MCS_PEO:
            return detail::verify_peo(g, mcs(g, MCSAlgorithm::BUCKET_MCS));
    }
    return ChordalResult();
}

} // namespace graph_recognition

#endif
