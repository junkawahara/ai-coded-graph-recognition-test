#ifndef GRAPH_RECOGNITION_UNICYCLIC_ENUM_H
#define GRAPH_RECOGNITION_UNICYCLIC_ENUM_H

/**
 * @file unicyclic_enum.h
 * @brief 非同型単閉路グラフ (unicyclic graph) の列挙
 *
 * 構成的列挙により頂点数 n の全非同型単閉路グラフを列挙する。
 * 単閉路グラフは連結で辺数 = 頂点数 (ちょうど 1 つの閉路を含む)。
 *
 * アルゴリズム:
 *   閉路長 g (3 <= g <= n) を固定し、各閉路頂点に非同型根付き木を付与。
 *   閉路の二面体対称性 (回転 + 反転) を考慮した bracelet 正規形により
 *   非同型グラフのみを出力する。
 *
 * 根付き木の生成には tree_enum.h の compute_rooted_trees を再利用する。
 *
 * 非同型数: OEIS A001429
 *   0, 0, 1, 2, 5, 13, 33, 89, 240, 657, 1806, ...
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
 * @brief 列挙された単閉路グラフ
 */
struct UnicyclicEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 単閉路グラフ列挙の結果
 */
struct UnicyclicEnumerationResult {
    std::vector<UnicyclicEnumeratedGraph> graphs;
};

namespace detail {

/**
 * @brief bracelet 正規性チェック
 *
 * 列 seq が二面体群 D_g (回転 + 反転) の全 2g 変換のうち
 * 辞書順最小であるか判定する。
 */
inline bool is_canonical_bracelet(const std::vector<int>& seq) {
    int g = (int)seq.size();
    for (int start = 1; start < g; ++start) {
        // 回転 by start
        for (int i = 0; i < g; ++i) {
            int a = seq[i];
            int b = seq[(i + start) % g];
            if (a < b) break;
            if (a > b) return false;
        }
    }
    for (int start = 0; start < g; ++start) {
        // 反転 + 回転 by start
        for (int i = 0; i < g; ++i) {
            int a = seq[i];
            int b = seq[((g - i) + start) % g];
            if (a < b) break;
            if (a > b) return false;
        }
    }
    return true;
}

/**
 * @brief 根付き木のレベル列から閉路頂点に付加する辺を構築
 *
 * @param L レベル列 (L[0]=0 が根)
 * @param cycle_vertex 閉路上の頂点番号 (1-indexed)
 * @param vertex_offset 追加頂点の開始番号 - 1
 * @param edges 出力先の辺リスト
 */
inline void attach_rooted_tree(
    const std::vector<int>& L,
    int cycle_vertex,
    int vertex_offset,
    std::vector<std::pair<int, int> >& edges) {

    int s = (int)L.size();
    if (s <= 1) return;  // 根のみ → 追加辺なし

    std::vector<int> depth_last(s, 0);
    depth_last[0] = cycle_vertex;

    for (int j = 1; j < s; ++j) {
        int actual_vertex = vertex_offset + j;
        int parent_vertex = depth_last[L[j] - 1];
        int u = (parent_vertex < actual_vertex) ? parent_vertex : actual_vertex;
        int v = (parent_vertex < actual_vertex) ? actual_vertex : parent_vertex;
        edges.push_back(std::make_pair(u, v));
        depth_last[L[j]] = actual_vertex;
    }
}

/**
 * @brief 根付き木 ID テーブル
 *
 * 全サイズの根付き木にグローバル ID を付与する構造。
 * ID はサイズ昇順、同サイズ内ではレベル列の辞書順。
 */
struct RootedTreeTable {
    /** id_offset[s] = サイズ s の最初の根付き木のグローバル ID */
    std::vector<int> id_offset;
    /** 根付き木キャッシュ (tree_enum.h から) */
    std::map<int, std::vector<std::vector<int> > > cache;
    /** 最大サイズ */
    int max_size;

    void build(int n) {
        max_size = n;
        for (int k = 1; k <= n; ++k) {
            compute_rooted_trees(k, cache);
        }
        id_offset.resize(n + 1, 0);
        int running = 0;
        for (int k = 1; k <= n; ++k) {
            id_offset[k] = running;
            running += (int)cache[k].size();
        }
    }

    int global_id(int size, int index) const {
        return id_offset[size] + index;
    }

    int tree_count(int size) const {
        std::map<int, std::vector<std::vector<int> > >::const_iterator it =
            cache.find(size);
        if (it == cache.end()) return 0;
        return (int)it->second.size();
    }

    const std::vector<int>& level_seq(int size, int index) const {
        return cache.find(size)->second[index];
    }
};

/**
 * @brief DFS で各閉路頂点への根付き木割り当てを列挙
 */
inline void unicyclic_enum_dfs(
    int pos, int remaining, int g,
    std::vector<int>& sizes,
    std::vector<int>& indices,
    std::vector<int>& id_seq,
    const RootedTreeTable& table,
    std::vector<UnicyclicEnumeratedGraph>& results,
    int n) {

    if (pos == g) {
        if (remaining != 0) return;
        if (!is_canonical_bracelet(id_seq)) return;

        // グラフ構築
        UnicyclicEnumeratedGraph graph;
        graph.n = n;

        // 閉路辺
        for (int i = 1; i < g; ++i) {
            graph.edges.push_back(std::make_pair(i, i + 1));
        }
        graph.edges.push_back(std::make_pair(1, g));

        // 各閉路頂点に根付き木を付加
        int vertex_offset = g;
        for (int i = 0; i < g; ++i) {
            int cycle_vertex = i + 1;
            const std::vector<int>& L = table.level_seq(sizes[i], indices[i]);
            attach_rooted_tree(L, cycle_vertex, vertex_offset, graph.edges);
            vertex_offset += sizes[i] - 1;
        }

        std::sort(graph.edges.begin(), graph.edges.end());
        results.push_back(graph);
        return;
    }

    int max_extra = remaining;
    // 最大サイズ = max_extra + 1
    int max_size = max_extra + 1;
    if (max_size > n) max_size = n;

    for (int s = 1; s <= max_size; ++s) {
        int count = table.tree_count(s);
        for (int idx = 0; idx < count; ++idx) {
            sizes[pos] = s;
            indices[pos] = idx;
            id_seq[pos] = table.global_id(s, idx);
            unicyclic_enum_dfs(
                pos + 1, remaining - (s - 1), g,
                sizes, indices, id_seq, table, results, n);
        }
    }
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型単閉路グラフを列挙する
 * @param n 頂点数
 * @return UnicyclicEnumerationResult
 *
 * 閉路長 g を 3 から n まで走査し、各閉路頂点に根付き木を
 * 構成的に割り当てる。閉路の二面体対称性を考慮した
 * bracelet 正規形により非同型グラフのみを出力する。
 */
inline UnicyclicEnumerationResult enumerate_unicyclic_graphs(int n) {
    UnicyclicEnumerationResult result;
    if (n < 3) return result;

    // 根付き木テーブル構築
    detail::RootedTreeTable table;
    table.build(n);

    for (int g = 3; g <= n; ++g) {
        int r = n - g;  // 追加頂点数
        std::vector<int> sizes(g, 0);
        std::vector<int> indices(g, 0);
        std::vector<int> id_seq(g, 0);
        detail::unicyclic_enum_dfs(
            0, r, g, sizes, indices, id_seq, table, result.graphs, n);
    }

    return result;
}

}  // namespace graph_recognition

#endif
