#ifndef GRAPH_RECOGNITION_CHAIN_ENUM_H
#define GRAPH_RECOGNITION_CHAIN_ENUM_H

/**
 * @file chain_enum.h
 * @brief 連鎖グラフ (chain graph) の列挙
 *
 * 階段型隣接行列 (staircase) の直接構築により頂点数 n の
 * 全非同型連鎖グラフを列挙する。chain グラフは unigraph
 * （次数列で一意に決まる）ため、次数列による重複排除で正しく動作する。
 *
 * 非同型数: OEIS A005418
 *   a(n) = 2^(n-2) + 2^(floor(n/2)-1) (n >= 2)
 */

#include <algorithm>
#include <cstddef>
#include <set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 連鎖グラフ列挙アルゴリズムの選択
 */
enum class ChainEnumAlgorithm {
    STAIRCASE /**< 階段型隣接行列の直接構築 */
};

/**
 * @brief 列挙されたグラフ
 */
struct ChainEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int>> edges;       /**< 辺リスト (u < v でソート済み) */
};

/**
 * @brief 連鎖グラフ列挙の結果
 */
struct ChainEnumerationResult {
    std::vector<ChainEnumeratedGraph> graphs;     /**< 列挙された連鎖グラフの配列 */
};

namespace detail {

/**
 * @brief 非減少列を再帰的に列挙し、各列から chain グラフを構築
 * @param pos 現在の位置 (0-indexed)
 * @param min_val 現在位置の最小値（非減少性を保証）
 * @param p X 側の頂点数
 * @param q Y 側の頂点数
 * @param current 現在構築中の非減少列
 * @param seen 既出の次数列の集合（重複排除用）
 * @param out 出力先
 * @param n 総頂点数
 */
inline void enumerate_staircases(
    int pos, int min_val, int p, int q,
    std::vector<int>& current,
    std::set<std::vector<int> >& seen,
    std::vector<ChainEnumeratedGraph>& out,
    int n)
{
    if (pos == p) {
        // Y 側の次数を計算: e_j = |{i : current[i] >= j}|
        // current は非減少なので二分探索で高速化
        std::vector<int> deg;
        deg.reserve(n);
        for (int i = 0; i < p; ++i) {
            deg.push_back(current[i]);
        }
        for (int j = 1; j <= q; ++j) {
            // current[i] >= j となる i の個数 = p - (lower_bound of j)
            int cnt = p - static_cast<int>(
                std::lower_bound(current.begin(), current.begin() + p, j)
                - current.begin());
            deg.push_back(cnt);
        }
        std::sort(deg.begin(), deg.end());

        if (seen.count(deg)) return;
        seen.insert(deg);

        // グラフ構築: x_i = vertex (i+1), y_j = vertex (p+j)
        // x_i は y_1, ..., y_{d_i} と隣接
        ChainEnumeratedGraph g;
        g.n = n;
        for (int i = 0; i < p; ++i) {
            for (int j = 1; j <= current[i]; ++j) {
                g.edges.push_back(std::make_pair(i + 1, p + j));
            }
        }
        out.push_back(g);
        return;
    }

    for (int val = min_val; val <= q; ++val) {
        current[pos] = val;
        enumerate_staircases(pos + 1, val, p, q, current, seen, out, n);
    }
}

}  // namespace detail

/**
 * @brief 頂点数 n の全非同型連鎖グラフを列挙する
 * @param n 頂点数
 * @param algo 使用するアルゴリズム (デフォルト: STAIRCASE)
 * @return ChainEnumerationResult
 *
 * 二部分割 (p, q) の全候補について階段型隣接行列を走査し、
 * 次数列による重複排除で非同型グラフのみを出力する。
 */
inline ChainEnumerationResult enumerate_chain_graphs(int n,
    ChainEnumAlgorithm algo = ChainEnumAlgorithm::STAIRCASE) {
    (void)algo;
    ChainEnumerationResult result;
    if (n <= 0) return result;

    std::set<std::vector<int> > seen;

    for (int p = 0; p <= n / 2; ++p) {
        int q = n - p;
        std::vector<int> current(p, 0);
        detail::enumerate_staircases(0, 0, p, q, current, seen, result.graphs, n);
    }

    return result;
}

}  // namespace graph_recognition

#endif
