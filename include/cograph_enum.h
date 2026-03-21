#ifndef GRAPH_RECOGNITION_COGRAPH_ENUM_H
#define GRAPH_RECOGNITION_COGRAPH_ENUM_H

/**
 * @file cograph_enum.h
 * @brief コグラフ (P4-free) の列挙 (cotree 再帰構築)
 *
 * 余木 (cotree) の再帰的構築により頂点集合 {1, ..., n} 上の
 * ラベル付きコグラフを全列挙する。
 *
 * Cograph は cotree で一意に表現される:
 *   - 葉は各頂点に対応
 *   - 内部ノードは 0 (disjoint union) または 1 (join) のタイプを持つ
 *   - 隣接する内部ノードは異なるタイプ (交互)
 *   - 各内部ノードは子を 2 個以上持つ
 *
 * 参考文献:
 *   - Seinsche, 1974 (P4-free 特性化)
 *   - Conte, Kante, Kurita, Uno, Wasa, DAM 2023 (proximity search)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Cograph 列挙アルゴリズムの選択
 */
enum class CographEnumAlgorithm {
    COTREE /**< cotree 再帰構築 */
};

/**
 * @brief Cograph 列挙の結果
 */
struct CographEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Cograph の配列 */
};

namespace detail {

/**
 * @brief 集合分割の再帰的生成
 *
 * restricted growth string に基づく。elems[0] は常に part 0 に配置。
 * 各後続要素は既存の part または新しい part に配置される。
 * k >= 2 の分割のみ出力する。
 */
inline void cograph_partition_dfs(
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
        cograph_partition_dfs(elems, idx + 1, parts, num_parts, out);
        parts[p].pop_back();
    }

    // 新しい part を開始
    parts[num_parts].push_back(elems[idx]);
    cograph_partition_dfs(elems, idx + 1, parts, num_parts + 1, out);
    parts[num_parts].pop_back();
}

/**
 * @brief 集合の全分割 (k >= 2) を生成する
 */
inline std::vector<std::vector<std::vector<int>>>
generate_cograph_partitions(const std::vector<int>& elems) {
    std::vector<std::vector<std::vector<int>>> result;
    if (elems.size() < 2) return result;

    std::vector<std::vector<int>> parts(elems.size());
    parts[0].push_back(elems[0]);
    cograph_partition_dfs(elems, 1, parts, 1, &result);
    parts[0].pop_back();

    return result;
}

/**
 * @brief デカルト積の再帰的生成
 *
 * groups[i] は i 番目の part の辺集合候補リスト。
 * 各 group から 1 つずつ選ぶ全組み合わせを生成する。
 */
inline void cograph_cartesian_dfs(
    const std::vector<std::vector<std::vector<std::pair<int, int>>>>& groups,
    std::size_t group_idx,
    std::vector<const std::vector<std::pair<int, int>>*>& current,
    const std::vector<std::vector<int>>& partition,
    int type,
    std::vector<std::vector<std::pair<int, int>>>* out) {

    if (group_idx == groups.size()) {
        // 辺を統合
        std::vector<std::pair<int, int>> merged;
        for (std::size_t i = 0; i < current.size(); ++i) {
            merged.insert(merged.end(), current[i]->begin(), current[i]->end());
        }

        // join (type 1) の場合: 異なる part 間に全辺を追加
        if (type == 1) {
            int k = (int)partition.size();
            for (int p1 = 0; p1 < k; ++p1) {
                for (int p2 = p1 + 1; p2 < k; ++p2) {
                    for (std::size_t a = 0; a < partition[p1].size(); ++a) {
                        for (std::size_t b = 0; b < partition[p2].size(); ++b) {
                            int u = partition[p1][a];
                            int v = partition[p2][b];
                            if (u > v) std::swap(u, v);
                            merged.push_back(std::make_pair(u, v));
                        }
                    }
                }
            }
        }

        std::sort(merged.begin(), merged.end());
        out->push_back(merged);
        return;
    }

    for (std::size_t i = 0; i < groups[group_idx].size(); ++i) {
        current.push_back(&groups[group_idx][i]);
        cograph_cartesian_dfs(groups, group_idx + 1, current, partition,
                              type, out);
        current.pop_back();
    }
}

/**
 * @brief 指定された頂点集合上の cograph を cotree root type で列挙する
 * @param vertices 頂点集合
 * @param type 0 = union (disjoint union), 1 = join
 * @param edge_sets 出力: 各 cograph の辺集合
 *
 * canonical cotree では隣接内部ノードのタイプが交互になるため、
 * 子には opposite type を適用する。
 */
inline void enumerate_cographs_by_type(
    const std::vector<int>& vertices, int type,
    std::vector<std::vector<std::pair<int, int>>>* edge_sets) {

    if (vertices.size() <= 1) {
        // 葉: 辺なし
        edge_sets->push_back(std::vector<std::pair<int, int>>());
        return;
    }

    // 頂点集合の全分割 (k >= 2)
    std::vector<std::vector<std::vector<int>>> partitions =
        generate_cograph_partitions(vertices);

    for (std::size_t pi = 0; pi < partitions.size(); ++pi) {
        const std::vector<std::vector<int>>& partition = partitions[pi];
        int k = (int)partition.size();

        // 各 part に対し opposite type で再帰
        std::vector<std::vector<std::vector<std::pair<int, int>>>> part_results(k);
        for (int p = 0; p < k; ++p) {
            enumerate_cographs_by_type(partition[p], 1 - type, &part_results[p]);
        }

        // デカルト積で全組み合わせを生成
        std::vector<const std::vector<std::pair<int, int>>*> current;
        cograph_cartesian_dfs(part_results, 0, current, partition, type,
                              edge_sets);
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Cograph を全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は COTREE のみ)
 * @return CographEnumerationResult
 *
 * cotree の再帰的構築による直接列挙。
 * n >= 2 では root type 0 (union) と 1 (join) の結果を統合する。
 */
inline CographEnumerationResult enumerate_cograph_graphs_cotree(int n,
    CographEnumAlgorithm algo = CographEnumAlgorithm::COTREE) {
    (void)algo;
    CographEnumerationResult result;
    if (n <= 0) return result;

    if (n == 1) {
        EnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    std::vector<int> vertices;
    for (int i = 1; i <= n; ++i) vertices.push_back(i);

    // root type 0 (union) と 1 (join) は n >= 2 で互いに素
    std::vector<std::vector<std::pair<int, int>>> type0;
    detail::enumerate_cographs_by_type(vertices, 0, &type0);

    std::vector<std::vector<std::pair<int, int>>> type1;
    detail::enumerate_cographs_by_type(vertices, 1, &type1);

    for (std::size_t i = 0; i < type0.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = type0[i];
        result.graphs.push_back(g);
    }
    for (std::size_t i = 0; i < type1.size(); ++i) {
        EnumeratedGraph g;
        g.n = n;
        g.edges = type1[i];
        result.graphs.push_back(g);
    }

    return result;
}

}  // namespace graph_recognition

#endif
