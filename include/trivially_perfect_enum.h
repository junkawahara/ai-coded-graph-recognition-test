#ifndef GRAPH_RECOGNITION_TRIVIALLY_PERFECT_ENUM_H
#define GRAPH_RECOGNITION_TRIVIALLY_PERFECT_ENUM_H

/**
 * @file trivially_perfect_enum.h
 * @brief 自明完全グラフ (trivially perfect graph) の列挙 (UVD 木構築)
 *
 * Universal Vertex Decomposition (UVD) 木の再帰的構築により
 * 頂点集合 {1, ..., n} 上のラベル付き自明完全グラフを全列挙する。
 *
 * Trivially perfect = chordal ∩ cograph = {P4, C4}-free。
 * 連結な trivially perfect グラフでは universal vertex の集合 U が
 * 非空であり、V \ U は 2 個以上の連結成分に分かれる。
 * この分解 (UVD) は一意であるため、重複なく構成的に列挙できる。
 *
 * 参考文献:
 *   - Golumbic, 1978 (trivially perfect 特性化)
 *   - Galvin, Wesley, Zacovic, JIS 25, 2022 (labeled 数え上げ)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Trivially Perfect 列挙アルゴリズムの選択
 */
enum class TriviallyPerfectEnumAlgorithm {
    UVD_TREE /**< UVD 木の再帰構築 */
};

/**
 * @brief Trivially Perfect 列挙の結果
 */
struct TriviallyPerfectEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された TP グラフの配列 */
};

namespace detail {

/**
 * @brief 集合分割の再帰的生成 (k >= 2)
 *
 * restricted growth string に基づく。elems[0] は常に part 0 に配置。
 * 各後続要素は既存の part または新しい part に配置される。
 * k >= 2 の分割のみ出力する。
 */
inline void tp_partition_dfs(
    const std::vector<int>& elems, std::size_t idx,
    std::vector<std::vector<int>>& parts, int num_parts,
    std::vector<std::vector<std::vector<int>>>* out) {

    if (idx == elems.size()) {
        if (num_parts >= 2) {
            out->push_back(
                std::vector<std::vector<int>>(parts.begin(),
                                              parts.begin() + num_parts));
        }
        return;
    }

    // 既存の part に追加
    for (int p = 0; p < num_parts; ++p) {
        parts[p].push_back(elems[idx]);
        tp_partition_dfs(elems, idx + 1, parts, num_parts, out);
        parts[p].pop_back();
    }

    // 新しい part を開始
    parts[num_parts].push_back(elems[idx]);
    tp_partition_dfs(elems, idx + 1, parts, num_parts + 1, out);
    parts[num_parts].pop_back();
}

/**
 * @brief 集合の全分割 (k >= 2) を生成する
 */
inline std::vector<std::vector<std::vector<int>>>
tp_generate_partitions_k2(const std::vector<int>& elems) {
    std::vector<std::vector<std::vector<int>>> result;
    if (elems.size() < 2) return result;

    std::vector<std::vector<int>> parts(elems.size());
    parts[0].push_back(elems[0]);
    tp_partition_dfs(elems, 1, parts, 1, &result);
    parts[0].pop_back();

    return result;
}

/**
 * @brief デカルト積の再帰的生成
 *
 * groups[i] は i 番目の part の辺集合候補リスト。
 * 各 group から 1 つずつ選ぶ全組み合わせを生成する。
 */
inline void tp_cartesian_dfs(
    const std::vector<std::vector<std::vector<std::pair<int, int>>>>& groups,
    std::size_t group_idx,
    std::vector<const std::vector<std::pair<int, int>>*>& current,
    std::vector<std::vector<std::pair<int, int>>>* out) {

    if (group_idx == groups.size()) {
        std::vector<std::pair<int, int>> merged;
        for (std::size_t i = 0; i < current.size(); ++i) {
            merged.insert(merged.end(), current[i]->begin(),
                          current[i]->end());
        }
        out->push_back(merged);
        return;
    }

    for (std::size_t i = 0; i < groups[group_idx].size(); ++i) {
        current.push_back(&groups[group_idx][i]);
        tp_cartesian_dfs(groups, group_idx + 1, current, out);
        current.pop_back();
    }
}

/**
 * @brief 連結 trivially perfect グラフを列挙する
 * @param vertices 頂点集合
 * @param edge_sets 出力: 各グラフの辺集合
 *
 * UVD 分解に基づく再帰構築:
 *   1. 全非空部分集合 U ⊆ vertices を universal vertex 集合として列挙
 *   2. R = vertices \ U が空なら完全グラフ
 *   3. R が非空なら R を k >= 2 パートに分割し、各パートで再帰
 *   4. 辺 = U のクリーク + U-R 間全辺 + 再帰内部辺
 */
inline void enumerate_connected_tp(
    const std::vector<int>& vertices,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    int sz = (int)vertices.size();

    // base case: 単一頂点
    if (sz == 1) {
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    // 全非空部分集合 U をビットマスクで列挙
    for (int mask = 1; mask < (1 << sz); ++mask) {
        std::vector<int> U, R;
        for (int i = 0; i < sz; ++i) {
            if (mask & (1 << i)) {
                U.push_back(vertices[i]);
            } else {
                R.push_back(vertices[i]);
            }
        }

        // U のクリーク辺を事前計算
        std::vector<std::pair<int, int>> u_clique;
        for (std::size_t a = 0; a < U.size(); ++a) {
            for (std::size_t b = a + 1; b < U.size(); ++b) {
                int x = U[a], y = U[b];
                if (x > y) std::swap(x, y);
                u_clique.push_back(std::make_pair(x, y));
            }
        }

        // U から R への全辺を事前計算
        std::vector<std::pair<int, int>> u_to_r;
        for (std::size_t a = 0; a < U.size(); ++a) {
            for (std::size_t b = 0; b < R.size(); ++b) {
                int x = U[a], y = R[b];
                if (x > y) std::swap(x, y);
                u_to_r.push_back(std::make_pair(x, y));
            }
        }

        if (R.empty()) {
            // U = V: 完全グラフ K_{|V|}
            std::vector<std::pair<int, int>> edges = u_clique;
            std::sort(edges.begin(), edges.end());
            edge_sets->push_back(edges);
            continue;
        }

        // R を k >= 2 パートに分割
        std::vector<std::vector<std::vector<int>>> partitions =
            tp_generate_partitions_k2(R);

        for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
            const std::vector<std::vector<int>>& partition = partitions[pi];
            int k = (int)partition.size();

            // 各パートで連結 TP を再帰列挙
            std::vector<std::vector<std::vector<std::pair<int, int>>>>
                part_results(k);
            for (int p = 0; p < k; ++p) {
                enumerate_connected_tp(partition[p], &part_results[p]);
            }

            // デカルト積
            std::vector<std::vector<std::pair<int, int>>> combined;
            std::vector<const std::vector<std::pair<int, int>>*> current;
            tp_cartesian_dfs(part_results, 0, current, &combined);

            // 各組み合わせに U のクリーク辺 + U-R 辺を追加
            for (std::size_t ci = 0; ci < combined.size(); ++ci) {
                std::vector<std::pair<int, int>> full_edges = combined[ci];
                full_edges.insert(full_edges.end(),
                                  u_clique.begin(), u_clique.end());
                full_edges.insert(full_edges.end(),
                                  u_to_r.begin(), u_to_r.end());
                std::sort(full_edges.begin(), full_edges.end());
                edge_sets->push_back(full_edges);
            }
        }
    }
}

/**
 * @brief 全 trivially perfect グラフを列挙する (非連結含む)
 * @param vertices 頂点集合
 * @param edge_sets 出力: 各グラフの辺集合
 *
 * 頂点集合を連結成分に分割し、各成分で連結 TP のデカルト積を取る。
 * k = 1 (連結) と k >= 2 (非連結) の両方を生成。
 */
inline void enumerate_all_tp(
    const std::vector<int>& vertices,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    if (vertices.empty()) {
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    // k = 1: 連結 TP グラフ
    enumerate_connected_tp(vertices, edge_sets);

    // k >= 2: 非連結 TP グラフ (成分間に辺なし)
    if (vertices.size() >= 2) {
        std::vector<std::vector<std::vector<int>>> partitions =
            tp_generate_partitions_k2(vertices);

        for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
            const std::vector<std::vector<int>>& partition = partitions[pi];
            int k = (int)partition.size();

            std::vector<std::vector<std::vector<std::pair<int, int>>>>
                part_results(k);
            for (int p = 0; p < k; ++p) {
                enumerate_connected_tp(partition[p], &part_results[p]);
            }

            std::vector<const std::vector<std::pair<int, int>>*> current;
            tp_cartesian_dfs(part_results, 0, current, edge_sets);
        }
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Trivially Perfect グラフを全列挙
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は UVD_TREE のみ)
 * @return TriviallyPerfectEnumerationResult
 *
 * UVD 木の再帰的構築による構成的列挙。
 * 連結成分分割 → 各成分で universal vertex 集合の選択 → 残余の分割 → 再帰。
 */
inline TriviallyPerfectEnumerationResult
enumerate_trivially_perfect_graphs_uvd(int n,
    TriviallyPerfectEnumAlgorithm algo =
        TriviallyPerfectEnumAlgorithm::UVD_TREE) {
    (void)algo;
    TriviallyPerfectEnumerationResult result;
    if (n <= 0) return result;

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<int> vertices;
    for (int i = 1; i <= n; ++i) vertices.push_back(i);

    std::vector<std::vector<std::pair<int, int>>> edge_sets;
    detail::enumerate_all_tp(vertices, &edge_sets);

    for (std::size_t i = 0; i < edge_sets.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = edge_sets[i];
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
