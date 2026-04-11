#ifndef GRAPH_RECOGNITION_FOREST_ENUM_H
#define GRAPH_RECOGNITION_FOREST_ENUM_H

/**
 * @file forest_enum.h
 * @brief 非同型森 (forest) の列挙
 *
 * 木列挙の合成により頂点数 n の全非同型森を列挙する。
 * 整数分割で連結成分のサイズ構成を列挙し、各サイズの
 * 非同型木を tree_enum.h から取得して組み合わせる。
 * 等サイズ成分では非減少インデックス制約により重複を排除する。
 *
 * 非同型数: OEIS A005195
 *   1, 1, 2, 3, 6, 10, 20, 37, 76, 153, ...
 *
 * 参考文献:
 *   Harary, Palmer, "Graphical Enumeration,"
 *   Academic Press, 1973
 */

#include "tree_enum.h"

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 森列挙アルゴリズムの選択
 */
enum class ForestEnumAlgorithm {
    PARTITION_COMPOSE /**< 整数分割 + 木の合成 */
};

/**
 * @brief 列挙された森グラフ
 */
struct ForestEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 森列挙の結果
 */
struct ForestEnumerationResult {
    std::vector<ForestEnumeratedGraph> graphs;    /**< 列挙された森の配列 */
};

namespace detail {

/**
 * @brief 各成分の木を選択して森を構築する再帰関数
 *
 * parts[idx..] の各パートサイズに対して木を選択し、
 * 等サイズパートでは非減少インデックス制約を適用する。
 *
 * @param parts 分割 (非増加順)
 * @param idx 現在処理中のパートインデックス
 * @param prev_index 等サイズの前パートで選んだ木のインデックス
 * @param vertex_offset 現在の頂点オフセット
 * @param current_edges 構築中の辺リスト
 * @param trees_by_size サイズ別の木リスト
 * @param results 結果格納先
 * @param n 総頂点数
 */
inline void forest_combine(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index, int vertex_offset,
    std::vector<std::pair<int, int> >& current_edges,
    const std::map<int, std::vector<TreeEnumeratedGraph> >& trees_by_size,
    std::vector<ForestEnumeratedGraph>& results,
    int n) {

    if (idx == parts.size()) {
        ForestEnumeratedGraph g;
        g.n = n;
        g.edges = current_edges;
        std::sort(g.edges.begin(), g.edges.end());
        results.push_back(g);
        return;
    }

    int sz = parts[idx];
    const std::vector<TreeEnumeratedGraph>& trees = trees_by_size.find(sz)->second;

    // 等サイズの前パートがある場合、インデックス >= prev_index に制限
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)trees.size(); ++i) {
        // 木の辺をオフセット付きで追加
        std::size_t old_size = current_edges.size();
        const std::vector<std::pair<int, int> >& tree_edges = trees[i].edges;
        for (std::size_t e = 0; e < tree_edges.size(); ++e) {
            int u = tree_edges[e].first + vertex_offset;
            int v = tree_edges[e].second + vertex_offset;
            current_edges.push_back(std::make_pair(u, v));
        }

        forest_combine(parts, idx + 1, i, vertex_offset + sz,
                        current_edges, trees_by_size, results, n);

        current_edges.resize(old_size);
    }
}

/**
 * @brief 整数分割を列挙し、各分割に対して森を構築
 *
 * n を非増加な正整数の和として表す全分割を列挙する。
 */
inline void forest_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    const std::map<int, std::vector<TreeEnumeratedGraph> >& trees_by_size,
    std::vector<ForestEnumeratedGraph>& results,
    int n) {

    if (remaining == 0) {
        std::vector<std::pair<int, int> > current_edges;
        forest_combine(parts, 0, 0, 0, current_edges, trees_by_size, results, n);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        forest_partition_dfs(remaining - s, s, parts, trees_by_size, results, n);
        parts.pop_back();
    }
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型森 (forest) を列挙する
 * @param n 頂点数
 * @param algo 使用するアルゴリズム (デフォルト: PARTITION_COMPOSE)
 * @return ForestEnumerationResult
 *
 * 整数分割で成分サイズ構成を列挙し、各サイズの非同型木を
 * 組み合わせて全非同型森を生成する。
 */
inline ForestEnumerationResult enumerate_forest_graphs(int n,
    ForestEnumAlgorithm algo = ForestEnumAlgorithm::PARTITION_COMPOSE) {
    (void)algo;
    ForestEnumerationResult result;
    if (n <= 0) return result;

    // 各サイズの非同型木を事前計算
    std::map<int, std::vector<TreeEnumeratedGraph> > trees_by_size;
    for (int k = 1; k <= n; ++k) {
        TreeEnumerationResult tres = enumerate_tree_graphs(k);
        trees_by_size[k] = tres.graphs;
    }

    // 整数分割を列挙して森を構築
    std::vector<int> parts;
    detail::forest_partition_dfs(n, n, parts, trees_by_size, result.graphs, n);

    return result;
}

}  // namespace graph_recognition

#endif
