#ifndef GRAPH_RECOGNITION_EULERIAN_ENUM_H
#define GRAPH_RECOGNITION_EULERIAN_ENUM_H

/**
 * @file eulerian_enum.h
 * @brief Eulerian グラフの列挙 (サイクル空間基底列挙)
 *
 * K_n のサイクル空間 (GF(2) ベクトル空間) の基底を用いて、
 * 頂点集合 {1, ..., n} 上のラベル付き Eulerian グラフを全列挙する。
 *
 * Eulerian グラフ（全頂点の次数が偶数）は K_n のサイクル空間を
 * 成すため、全域木の基本サイクルの部分集合の対称差として
 * 一意に表現できる。基底の次元は d = (n-1)(n-2)/2 であり、
 * 全 2^d 個のラベル付き Eulerian グラフを無駄なく列挙する。
 *
 * 参考文献:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief Eulerian 列挙アルゴリズムの選択
 */
enum class EulerianEnumAlgorithm {
    CYCLE_SPACE_BASIS /**< サイクル空間基底列挙 */
};

/**
 * @brief Eulerian 列挙の結果
 */
struct EulerianEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された Eulerian グラフの配列 */
};

namespace detail {

/** @brief サイクル空間基底列挙の内部状態 */
struct EulerianEnumState {
    int n;
    int num_edges;   /**< C(n,2): K_n の辺数 */
    int num_cycles;  /**< d = (n-1)(n-2)/2: サイクル空間の次元 */
    std::vector<std::pair<int, int>> edge_list;       /**< K_n の全辺 (u<v, 辞書順) */
    std::vector<std::vector<int>> edge_idx;           /**< edge_idx[i][j] = 辺 (i,j) のインデックス */
    std::vector<std::vector<int>> fundamental_cycles; /**< 各基本サイクルの辺インデックスリスト */
    std::vector<char> edge_present;                   /**< 現在の辺集合 (XOR 状態) */
};

/**
 * @brief 状態の初期化: 辺リスト・基本サイクルを構築
 */
inline EulerianEnumState eulerian_build_state(int n) {
    EulerianEnumState state;
    state.n = n;

    // K_n の全辺を辞書順に列挙
    state.edge_idx.assign(n + 1, std::vector<int>(n + 1, -1));
    int idx = 0;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.edge_list.push_back(std::make_pair(i, j));
            state.edge_idx[i][j] = idx;
            state.edge_idx[j][i] = idx;
            ++idx;
        }
    }
    state.num_edges = idx;
    state.edge_present.assign(idx, 0);

    // 全域木: パス 1-2-3-...-n (辺: (k, k+1) for k=1..n-1)
    // 余木辺: (i,j) で j > i+1 のもの
    // 各余木辺 (i,j) の基本サイクル: パス辺 (i,i+1),(i+1,i+2),...,(j-1,j) + 辺 (i,j)
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 2; j <= n; ++j) {
            std::vector<int> cycle;
            for (int k = i; k < j; ++k) {
                cycle.push_back(state.edge_idx[k][k + 1]);
            }
            cycle.push_back(state.edge_idx[i][j]);
            state.fundamental_cycles.push_back(cycle);
        }
    }
    state.num_cycles = static_cast<int>(state.fundamental_cycles.size());

    return state;
}

/**
 * @brief サイクル空間基底列挙の DFS
 *
 * cycle_idx 番目の基本サイクルを含むか含まないかで二分岐し、
 * 全基本サイクルについて選択が完了したら辺集合を出力する。
 */
inline void eulerian_enum_dfs(EulerianEnumState& state, int cycle_idx,
                              std::vector<EnumeratedGraph>* out) {
    if (cycle_idx == state.num_cycles) {
        EnumeratedGraph g;
        g.n = state.n;
        for (int e = 0; e < state.num_edges; ++e) {
            if (state.edge_present[e]) {
                g.edges.push_back(state.edge_list[e]);
            }
        }
        out->push_back(g);
        return;
    }

    // 分岐 1: この基本サイクルを含まない
    eulerian_enum_dfs(state, cycle_idx + 1, out);

    // 分岐 2: この基本サイクルを含む (XOR トグル)
    const std::vector<int>& cycle = state.fundamental_cycles[cycle_idx];
    for (size_t i = 0; i < cycle.size(); ++i) {
        state.edge_present[cycle[i]] ^= 1;
    }
    eulerian_enum_dfs(state, cycle_idx + 1, out);
    // 元に戻す
    for (size_t i = 0; i < cycle.size(); ++i) {
        state.edge_present[cycle[i]] ^= 1;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き Eulerian グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は CYCLE_SPACE_BASIS のみ)
 * @return EulerianEnumerationResult
 *
 * K_n のサイクル空間は GF(2) 上の (n-1)(n-2)/2 次元ベクトル空間を
 * 成す。全域木パス 1-2-...-n の基本サイクルを基底として、
 * 全 2^d 個の部分集合の対称差を列挙する。
 *
 * 各グラフは全頂点の次数が偶数であることが代数的に保証される。
 */
inline EulerianEnumerationResult
enumerate_eulerian_graphs(int n,
    EulerianEnumAlgorithm algo =
        EulerianEnumAlgorithm::CYCLE_SPACE_BASIS) {
    (void)algo;
    EulerianEnumerationResult result;
    if (n < 0) return result;
    if (n <= 1) {
        EnumeratedGraph g;
        g.n = n;
        result.graphs.push_back(g);
        return result;
    }
    detail::EulerianEnumState state = detail::eulerian_build_state(n);
    detail::eulerian_enum_dfs(state, 0, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
