#ifndef GRAPH_RECOGNITION_THRESHOLD_ENUM_H
#define GRAPH_RECOGNITION_THRESHOLD_ENUM_H

/**
 * @file threshold_enum.h
 * @brief 閾値グラフ (threshold graph) の列挙
 *
 * バイナリ文字列による直接構築で頂点数 n の全非同型
 * 閾値グラフを列挙する。各ステップで孤立点 (0) または
 * 支配点 (1) を追加し、2^(n-1) 個のグラフを生成する。
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 閾値グラフ列挙アルゴリズムの選択
 */
enum class ThresholdEnumAlgorithm {
    BINARY_STRING /**< バイナリ文字列からの直接構築 */
};

/**
 * @brief 列挙されたグラフ
 */
struct ThresholdEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int>> edges;       /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 閾値グラフ列挙の結果
 */
struct ThresholdEnumerationResult {
    std::vector<ThresholdEnumeratedGraph> graphs;  /**< 列挙された閾値グラフの配列 */
};

namespace detail {

/**
 * @brief バイナリ文字列 (ビットマスク) から閾値グラフを構築
 * @param n 頂点数
 * @param mask 長さ n-1 のビットマスク (ビット i: 頂点 i+2 が支配点なら 1)
 * @return 辺リスト (u < v でソート済み)
 */
inline std::vector<std::pair<int, int>> build_threshold_graph(int n, unsigned long long mask) {
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < n - 1; ++i) {
        if (mask & (1ULL << i)) {
            // 頂点 i+2 は支配点: 既存頂点 1..i+1 と隣接
            for (int j = 1; j <= i + 1; ++j) {
                edges.push_back(std::make_pair(j, i + 2));
            }
        }
    }
    return edges;
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型閾値グラフを列挙する
 * @param n 頂点数
 * @param algo 使用するアルゴリズム (デフォルト: BINARY_STRING)
 * @return ThresholdEnumerationResult
 *
 * バイナリ文字列特性化により 2^(n-1) 個の非同型閾値グラフを
 * 直接構築する。各文字列のビット i は頂点 i+2 が支配点 (1) か
 * 孤立点 (0) かを表す。
 */
inline ThresholdEnumerationResult enumerate_threshold_graphs(int n,
    ThresholdEnumAlgorithm algo = ThresholdEnumAlgorithm::BINARY_STRING) {
    (void)algo;
    ThresholdEnumerationResult result;
    if (n <= 0) return result;

    if (n > 64) return result;
    unsigned long long total = 1ULL << (n - 1);  // 2^(n-1)
    result.graphs.reserve(static_cast<std::size_t>(total));

    for (unsigned long long mask = 0; mask < total; ++mask) {
        ThresholdEnumeratedGraph g;
        g.n = n;
        g.edges = detail::build_threshold_graph(n, mask);
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
