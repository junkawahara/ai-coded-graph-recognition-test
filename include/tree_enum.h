#ifndef GRAPH_RECOGNITION_TREE_ENUM_H
#define GRAPH_RECOGNITION_TREE_ENUM_H

/**
 * @file tree_enum.h
 * @brief 非同型自由木 (free tree) の列挙
 *
 * ボトムアップ再帰構成 + 重心分解により頂点数 n の
 * 全非同型自由木を列挙する。
 *
 * 根付き木をレベル列 (DFS preorder での深さ列) で表現し、
 * 整数分割 + メモ化再帰で全正規根付き木を生成する。
 * 自由木は重心 (centroid) で根付けすることで一意に表現される:
 *   - 単重心木: 根の全部分木サイズ <= floor((n-1)/2)
 *   - 双重心木 (n 偶数): サイズ n/2 の根付き木ペア (T_i, T_j), i <= j
 *
 * 非同型数: OEIS A000055
 *   1, 1, 1, 2, 3, 6, 11, 23, 47, 106, ...
 *
 * 参考文献:
 *   Wright, Richmond, Odlyzko, McKay,
 *   "Constant Time Generation of Free Trees,"
 *   SIAM J. Comput. 15(2), 1986
 *
 *   Beyer, Hedetniemi,
 *   "Constant Time Generation of Rooted Trees,"
 *   SIAM J. Comput. 9(4), 1980
 */

#include <algorithm>
#include <cstddef>
#include <map>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 木列挙アルゴリズムの選択
 */
enum class TreeEnumAlgorithm {
    LEVEL_SEQUENCE /**< レベル列による構成的列挙 */
};

/**
 * @brief 列挙された木グラフ
 */
struct TreeEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 木列挙の結果
 */
struct TreeEnumerationResult {
    std::vector<TreeEnumeratedGraph> graphs;      /**< 列挙された木の配列 */
};

namespace detail {

/**
 * @brief レベル列から辺リストを構築 (単一の根付き木)
 *
 * レベル列 L (0-indexed entries, L[0]=0) から 1-indexed 頂点の辺リストを生成。
 * 頂点 i+1 の親は、直前の深さ L[i]-1 の頂点。
 */
inline TreeEnumeratedGraph level_seq_to_tree_graph(
    const std::vector<int>& L, int n) {

    TreeEnumeratedGraph g;
    g.n = n;
    if (n <= 1) return g;

    // depth_last[d] = 深さ d に最後に現れた頂点 (1-indexed)
    std::vector<int> depth_last(n, 0);
    depth_last[0] = 1;

    for (int i = 1; i < n; ++i) {
        int vertex = i + 1;
        int parent = depth_last[L[i] - 1];
        int u = (parent < vertex) ? parent : vertex;
        int v = (parent < vertex) ? vertex : parent;
        g.edges.push_back(std::make_pair(u, v));
        depth_last[L[i]] = vertex;
    }

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief 2 つの根付き木を根で結合して双重心木を構築
 *
 * T1 の頂点: 1..half, T2 の頂点: half+1..2*half
 * 橋辺: (1, half+1)
 */
inline TreeEnumeratedGraph join_rooted_trees(
    const std::vector<int>& L1, const std::vector<int>& L2) {

    int half = (int)L1.size();
    TreeEnumeratedGraph g;
    g.n = 2 * half;

    // T1 の辺 (頂点 1..half)
    {
        std::vector<int> depth_last(half, 0);
        depth_last[0] = 1;
        for (int i = 1; i < half; ++i) {
            int vertex = i + 1;
            int parent = depth_last[L1[i] - 1];
            int u = (parent < vertex) ? parent : vertex;
            int v = (parent < vertex) ? vertex : parent;
            g.edges.push_back(std::make_pair(u, v));
            depth_last[L1[i]] = vertex;
        }
    }

    // T2 の辺 (頂点 half+1..2*half)
    {
        std::vector<int> depth_last(half, 0);
        depth_last[0] = half + 1;
        for (int i = 1; i < half; ++i) {
            int vertex = half + i + 1;
            int parent = depth_last[L2[i] - 1];
            int u = (parent < vertex) ? parent : vertex;
            int v = (parent < vertex) ? vertex : parent;
            g.edges.push_back(std::make_pair(u, v));
            depth_last[L2[i]] = vertex;
        }
    }

    // 橋辺
    g.edges.push_back(std::make_pair(1, half + 1));

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief 根の直接の子の部分木サイズを計算
 */
inline std::vector<int> root_subtree_sizes(const std::vector<int>& L, int n) {
    std::vector<int> sizes;
    if (n <= 1) return sizes;

    int i = 1;
    while (i < n) {
        // L[i] == 1 なので、ここから新しい部分木が始まる
        int start = i;
        ++i;
        while (i < n && L[i] > 1) {
            ++i;
        }
        sizes.push_back(i - start);
    }
    return sizes;
}

/**
 * @brief レベル列を組み立てる (根 + 部分木列)
 *
 * 部分木のレベル列を +1 シフトして連結する。
 */
inline std::vector<int> assemble_level_sequence(
    const std::vector<std::vector<int> >& subtrees) {

    int total = 1;
    for (std::size_t i = 0; i < subtrees.size(); ++i) {
        total += (int)subtrees[i].size();
    }

    std::vector<int> L;
    L.reserve(total);
    L.push_back(0);  // 根

    for (std::size_t i = 0; i < subtrees.size(); ++i) {
        const std::vector<int>& sub = subtrees[i];
        for (std::size_t j = 0; j < sub.size(); ++j) {
            L.push_back(sub[j] + 1);
        }
    }

    return L;
}

/**
 * @brief 部分木の組み合わせを再帰的に生成
 *
 * parts[idx..] の各パートサイズに対して根付き木を選択し、
 * 等サイズパートでは非減少インデックス制約を適用する。
 */
inline void tree_enum_combine_subtrees(
    const std::vector<int>& parts, std::size_t idx,
    int prev_index,
    std::vector<std::vector<int> >& chosen,
    std::map<int, std::vector<std::vector<int> > >& cache,
    std::vector<std::vector<int> >& results) {

    if (idx == parts.size()) {
        results.push_back(assemble_level_sequence(chosen));
        return;
    }

    int sz = parts[idx];
    const std::vector<std::vector<int> >& trees = cache[sz];

    // 等サイズの前パートがある場合、インデックス >= prev_index に制限
    int start = 0;
    if (idx > 0 && parts[idx] == parts[idx - 1]) {
        start = prev_index;
    }

    for (int i = start; i < (int)trees.size(); ++i) {
        chosen.push_back(trees[i]);
        tree_enum_combine_subtrees(parts, idx + 1, i, chosen, cache, results);
        chosen.pop_back();
    }
}

/**
 * @brief 整数分割を列挙し、各分割に対して部分木の組み合わせを生成
 */
inline void tree_enum_partition_dfs(
    int remaining, int max_part,
    std::vector<int>& parts,
    std::map<int, std::vector<std::vector<int> > >& cache,
    std::vector<std::vector<int> >& results) {

    if (remaining == 0) {
        std::vector<std::vector<int> > chosen;
        tree_enum_combine_subtrees(parts, 0, 0, chosen, cache, results);
        return;
    }

    int upper = (remaining < max_part) ? remaining : max_part;
    for (int s = upper; s >= 1; --s) {
        parts.push_back(s);
        tree_enum_partition_dfs(remaining - s, s, parts, cache, results);
        parts.pop_back();
    }
}

/**
 * @brief サイズ k の全正規根付き木をレベル列として生成 (メモ化)
 */
inline void compute_rooted_trees(
    int k,
    std::map<int, std::vector<std::vector<int> > >& cache) {

    if (cache.count(k)) return;

    if (k == 1) {
        std::vector<int> single(1, 0);
        cache[1].push_back(single);
        return;
    }

    // 小さいサイズを先に計算
    for (int i = 1; i < k; ++i) {
        compute_rooted_trees(i, cache);
    }

    // (k-1) の整数分割を列挙し、各分割に対して部分木を組み合わせる
    std::vector<int> parts;
    tree_enum_partition_dfs(k - 1, k - 1, parts, cache, cache[k]);

    // 辞書順ソート
    std::sort(cache[k].begin(), cache[k].end());
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型自由木 (free tree) を列挙する
 * @param n 頂点数
 * @param algo 使用するアルゴリズム (デフォルト: LEVEL_SEQUENCE)
 * @return TreeEnumerationResult
 *
 * 根付き木のメモ化再帰生成 + 重心分解により、
 * 各同型類につき正確に 1 つの代表元を出力する。
 */
inline TreeEnumerationResult enumerate_tree_graphs(int n,
    TreeEnumAlgorithm algo = TreeEnumAlgorithm::LEVEL_SEQUENCE) {
    (void)algo;
    TreeEnumerationResult result;
    if (n <= 0) return result;

    if (n == 1) {
        TreeEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 2) {
        TreeEnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }

    // 全サイズの根付き木を生成
    std::map<int, std::vector<std::vector<int> > > cache;
    for (int k = 1; k <= n; ++k) {
        detail::compute_rooted_trees(k, cache);
    }

    // --- 単重心木 ---
    // cache[n] から、根の全部分木サイズが <= floor((n-1)/2) のものを抽出
    int threshold = (n - 1) / 2;
    const std::vector<std::vector<int> >& rooted_n = cache[n];
    for (std::size_t t = 0; t < rooted_n.size(); ++t) {
        std::vector<int> sizes = detail::root_subtree_sizes(rooted_n[t], n);
        int max_sz = 0;
        for (std::size_t i = 0; i < sizes.size(); ++i) {
            if (sizes[i] > max_sz) max_sz = sizes[i];
        }
        if (max_sz <= threshold) {
            result.graphs.push_back(
                detail::level_seq_to_tree_graph(rooted_n[t], n));
        }
    }

    // --- 双重心木 (n が偶数のみ) ---
    if (n % 2 == 0) {
        int half = n / 2;
        const std::vector<std::vector<int> >& rooted_half = cache[half];
        int cnt = (int)rooted_half.size();
        for (int i = 0; i < cnt; ++i) {
            for (int j = i; j < cnt; ++j) {
                result.graphs.push_back(
                    detail::join_rooted_trees(rooted_half[i], rooted_half[j]));
            }
        }
    }

    return result;
}

}  // namespace graph_recognition

#endif
