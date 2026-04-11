#ifndef GRAPH_RECOGNITION_CATERPILLAR_ENUM_H
#define GRAPH_RECOGNITION_CATERPILLAR_ENUM_H

/**
 * @file caterpillar_enum.h
 * @brief キャタピラー木 (caterpillar tree) の列挙
 *
 * spine + 葉の配分による構成的列挙により頂点数 n の
 * 全非同型キャタピラー木を列挙する。
 *
 * キャタピラー木は全頂点が中心パス (spine) から距離 1 以内にある木。
 * pathwidth 1 のグラフと一致する。葉数列 (d_1, ..., d_s) と
 * その反転でキャタピラーの同型類が一意に決まる。
 *
 * 非同型数: OEIS A000672
 *   1, 1, 1, 2, 3, 6, 10, 20, 36, 72, ...
 *
 * 参考文献:
 *   Harary, Schwenk, "The number of caterpillars,"
 *   Discrete Math. 6(4), 1973
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief キャタピラー列挙アルゴリズムの選択
 */
enum class CaterpillarEnumAlgorithm {
    CONSTRUCTIVE /**< spine + 葉配分の構成的列挙 */
};

/**
 * @brief 列挙されたグラフ
 */
struct CaterpillarEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > edges;      /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief キャタピラー列挙の結果
 */
struct CaterpillarEnumerationResult {
    std::vector<CaterpillarEnumeratedGraph> graphs; /**< 列挙されたキャタピラーの配列 */
};

namespace detail {

/**
 * @brief 葉数列が回文正規形か判定
 *
 * spine の反転対称性による重複を排除するため、
 * seq <= reverse(seq) (辞書順) の場合のみ正規形とみなす。
 */
inline bool is_palindrome_canonical(const std::vector<int>& seq) {
    int s = (int)seq.size();
    for (int i = 0; i < s / 2; ++i) {
        if (seq[i] < seq[s - 1 - i]) return true;
        if (seq[i] > seq[s - 1 - i]) return false;
    }
    return true;  // palindrome
}

/**
 * @brief 葉数列からキャタピラーグラフを構築
 *
 * spine 頂点を 1, 2, ..., s とし、葉を s+1 から順に割り当てる。
 */
inline CaterpillarEnumeratedGraph build_caterpillar(
    int n, const std::vector<int>& leaf_counts) {

    int s = (int)leaf_counts.size();
    CaterpillarEnumeratedGraph g;
    g.n = n;

    // Spine edges: 1-2, 2-3, ..., (s-1)-s
    for (int i = 1; i < s; ++i) {
        g.edges.push_back(std::make_pair(i, i + 1));
    }

    // Leaf edges
    int next_leaf = s + 1;
    for (int i = 0; i < s; ++i) {
        int spine_v = i + 1;
        for (int j = 0; j < leaf_counts[i]; ++j) {
            g.edges.push_back(std::make_pair(spine_v, next_leaf));
            ++next_leaf;
        }
    }

    std::sort(g.edges.begin(), g.edges.end());
    return g;
}

/**
 * @brief 制約付き組成を再帰的に列挙
 *
 * r 個の葉を s 個の spine 頂点に分配する。
 * 制約: d_1 >= 1, d_s >= 1, d_i >= 0 (2 <= i <= s-1)
 * 回文正規形のもののみ出力する。
 */
inline void enumerate_compositions_dfs(
    int pos, int remaining, int s,
    std::vector<int>& current,
    std::vector<CaterpillarEnumeratedGraph>& out,
    int n) {

    if (pos == s) {
        if (remaining != 0) return;
        if (!is_palindrome_canonical(current)) return;
        out.push_back(build_caterpillar(n, current));
        return;
    }

    int min_val = (pos == 0 || pos == s - 1) ? 1 : 0;

    // Future positions need at least this much total
    int future_min = (pos < s - 1) ? 1 : 0;
    int max_val = remaining - future_min;

    for (int d = min_val; d <= max_val; ++d) {
        current[pos] = d;
        enumerate_compositions_dfs(pos + 1, remaining - d, s, current, out, n);
    }
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型キャタピラー木を列挙する
 * @param n 頂点数
 * @param algo 使用するアルゴリズム (デフォルト: CONSTRUCTIVE)
 * @return CaterpillarEnumerationResult
 *
 * spine 長 s を 1 から n-2 まで走査し、各 spine 長について
 * 葉の配分を列挙する。spine の反転対称性を考慮した
 * 回文正規形により非同型グラフのみを出力する。
 */
inline CaterpillarEnumerationResult enumerate_caterpillar_graphs(int n,
    CaterpillarEnumAlgorithm algo = CaterpillarEnumAlgorithm::CONSTRUCTIVE) {
    (void)algo;
    CaterpillarEnumerationResult result;
    if (n <= 0) return result;

    if (n == 1) {
        CaterpillarEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    if (n == 2) {
        CaterpillarEnumeratedGraph g;
        g.n = 2;
        g.edges.push_back(std::make_pair(1, 2));
        result.graphs.push_back(g);
        return result;
    }

    // n >= 3
    for (int s = 1; s <= n - 2; ++s) {
        int r = n - s;  // number of leaves

        if (s == 1) {
            // Star K_{1,r}
            std::vector<int> leaf_counts(1, r);
            result.graphs.push_back(detail::build_caterpillar(n, leaf_counts));
        } else {
            // s >= 2: need r >= 2 (d_1 >= 1, d_s >= 1)
            if (r < 2) continue;
            std::vector<int> current(s, 0);
            detail::enumerate_compositions_dfs(
                0, r, s, current, result.graphs, n);
        }
    }

    return result;
}

}  // namespace graph_recognition

#endif
