#ifndef GRAPH_RECOGNITION_CLUSTER_ENUM_H
#define GRAPH_RECOGNITION_CLUSTER_ENUM_H

/**
 * @file cluster_enum.h
 * @brief Cluster グラフ (P3-free / クリーク非交和) のラベル付き全列挙
 *
 * 集合分割 (set partition) の再帰的列挙により、
 * n 頂点のラベル付き cluster グラフを全て構成的に列挙する。
 * 各集合分割が一意の cluster グラフに対応するため、
 * 重複除去は不要。列挙数は Bell 数 B(n) (OEIS A000110)。
 */

#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙された cluster グラフ
 */
struct ClusterEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief Cluster グラフ列挙の結果
 */
struct ClusterEnumerationResult {
    std::vector<ClusterEnumeratedGraph> graphs;   /**< 列挙された cluster グラフの配列 */
};

namespace detail_cluster_enum {

/**
 * @brief 集合分割からグラフを構築する (内部関数)
 * @param n 頂点数
 * @param blocks 分割ブロック (各ブロックは頂点の集合)
 * @return ClusterEnumeratedGraph
 *
 * 各ブロック内の全頂点ペアを辺として追加 (完全グラフ)。
 */
inline ClusterEnumeratedGraph build_cluster_graph(
    int n,
    const std::vector<std::vector<int> >& blocks) {
    ClusterEnumeratedGraph g;
    g.n = n;
    for (size_t b = 0; b < blocks.size(); ++b) {
        const std::vector<int>& block = blocks[b];
        for (size_t i = 0; i < block.size(); ++i) {
            for (size_t j = i + 1; j < block.size(); ++j) {
                int u = block[i];
                int v = block[j];
                if (u > v) {
                    int tmp = u;
                    u = v;
                    v = tmp;
                }
                g.edges.push_back(std::make_pair(u, v));
            }
        }
    }
    return g;
}

/**
 * @brief 集合分割の再帰的列挙 (内部関数)
 * @param vertex 現在配置する頂点 (1-indexed)
 * @param n 頂点数
 * @param blocks 現在の分割ブロック
 * @param out 結果を格納するベクタ
 *
 * 頂点を 1 から n まで順に処理し、各頂点を
 * 既存ブロックに追加するか、新ブロックを作成する。
 */
inline void set_partition_dfs(
    int vertex,
    int n,
    std::vector<std::vector<int> >& blocks,
    std::vector<ClusterEnumeratedGraph>& out) {
    if (vertex > n) {
        out.push_back(build_cluster_graph(n, blocks));
        return;
    }

    // 既存ブロックに追加
    size_t num_blocks = blocks.size();
    for (size_t i = 0; i < num_blocks; ++i) {
        blocks[i].push_back(vertex);
        set_partition_dfs(vertex + 1, n, blocks, out);
        blocks[i].pop_back();
    }

    // 新しいブロックを作成
    std::vector<int> new_block;
    new_block.push_back(vertex);
    blocks.push_back(new_block);
    set_partition_dfs(vertex + 1, n, blocks, out);
    blocks.pop_back();
}

} // namespace detail_cluster_enum

/**
 * @brief 頂点数 n のラベル付き cluster グラフを全列挙する
 * @param n 頂点数
 * @return ClusterEnumerationResult
 *
 * 集合分割の再帰的列挙により B(n) 個のグラフを構成する。
 */
inline ClusterEnumerationResult enumerate_cluster_graphs(int n) {
    ClusterEnumerationResult result;
    if (n <= 0) return result;

    std::vector<std::vector<int> > blocks;
    detail_cluster_enum::set_partition_dfs(1, n, blocks, result.graphs);

    return result;
}

} // namespace graph_recognition

#endif
