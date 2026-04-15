#ifndef GRAPH_RECOGNITION_KREGULAR_H
#define GRAPH_RECOGNITION_KREGULAR_H

/**
 * @file kregular.h
 * @brief k-正則グラフ (k-regular graph) 認識
 *
 * 全頂点の次数が同一であることを確認する。
 */

#include "graph.h"

namespace graph_recognition {

/**
 * @brief k-正則グラフ認識アルゴリズムの選択
 */
enum class KRegularAlgorithm {
    DEGREE_CHECK /**< 次数チェック */
};

/**
 * @brief k-正則グラフ認識の結果
 */
struct KRegularResult {
    bool is_kregular = false; /**< k-正則であれば true */
    int k = -1;              /**< 正則次数 (正則でない場合は -1) */
};

/**
 * @brief グラフが正則グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: DEGREE_CHECK)
 * @return KRegularResult
 *
 * 正則グラフ ⟺ 全頂点の次数が同一。
 */
inline KRegularResult check_kregular(const Graph& g,
    KRegularAlgorithm algo = KRegularAlgorithm::DEGREE_CHECK) {
    (void)algo;
    KRegularResult res;

    int n = g.n;
    if (n == 0) {
        res.is_kregular = true;
        res.k = 0;
        return res;
    }

    int deg = (int)g.adj[1].size();
    for (int v = 2; v <= n; ++v) {
        if ((int)g.adj[v].size() != deg) return res;
    }

    res.is_kregular = true;
    res.k = deg;
    return res;
}

} // namespace graph_recognition

#endif
