#ifndef GRAPH_RECOGNITION_DIGRAPH_ENUM_H
#define GRAPH_RECOGNITION_DIGRAPH_ENUM_H

/**
 * @file digraph_enum.h
 * @brief Digraph (有向グラフ) のラベル付き全列挙
 *
 * 頂点集合 {1, ..., n} 上の全ラベル付き単純有向グラフ (自己ループなし)
 * を構成的に全列挙する。n(n-1)/2 個の非順序ペア {i, j} それぞれについて
 * 4 通り (辺なし / i->j / j->i / 両方向) を DFS で選択し、
 * 全 4^(n(n-1)/2) = 2^(n(n-1)) 個のラベル付き有向グラフを重複なく列挙する。
 *
 * 構成の各段階で得られるグラフは定義上必ず有向グラフであるため、
 * 認識フィルタは不要 (構成的列挙)。
 *
 * 参考文献:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 *   - McKay, nauty User's Guide (directg)
 * OEIS:
 *   - A000273 (非ラベル付き有向グラフ数)
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙された有向グラフ
 */
struct DigraphEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > arcs;       /**< 有向辺リスト (u, v) = u->v, ソート済み */
};

/**
 * @brief Digraph 列挙の結果
 */
struct DigraphEnumerationResult {
    std::vector<DigraphEnumeratedGraph> graphs;   /**< 列挙された有向グラフの配列 */
};

namespace detail_digraph_enum {

/** @brief 構成的列挙の内部状態 */
struct DigraphEnumState {
    int n;                                         /**< 頂点数 */
    int num_pairs;                                 /**< C(n,2): 非順序ペア数 */
    std::vector<std::pair<int, int> > pairs;       /**< 全非順序ペア (i<j, 辞書順) */
    std::vector<std::pair<int, int> > current_arcs; /**< 現在選択中のアーク列 */
};

/**
 * @brief 状態の初期化: 全非順序ペアを構築
 */
inline DigraphEnumState digraph_build_state(int n) {
    DigraphEnumState state;
    state.n = n;
    for (int i = 1; i <= n; ++i) {
        for (int j = i + 1; j <= n; ++j) {
            state.pairs.push_back(std::make_pair(i, j));
        }
    }
    state.num_pairs = static_cast<int>(state.pairs.size());
    return state;
}

/**
 * @brief 構成的列挙の DFS
 *
 * pair_idx 番目の非順序ペア (i, j) について 4 分岐で探索:
 *   分岐 0: 辺なし
 *   分岐 1: アーク i->j のみ
 *   分岐 2: アーク j->i のみ
 *   分岐 3: アーク i->j と j->i の両方
 * 全ペアの選択が決定したら有向グラフを出力する。
 */
inline void digraph_enum_dfs(DigraphEnumState& state, int pair_idx,
                             std::vector<DigraphEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        DigraphEnumeratedGraph g;
        g.n = state.n;
        g.arcs = state.current_arcs;
        std::sort(g.arcs.begin(), g.arcs.end());
        out->push_back(g);
        return;
    }

    int i = state.pairs[pair_idx].first;
    int j = state.pairs[pair_idx].second;

    // 分岐 0: 辺なし
    digraph_enum_dfs(state, pair_idx + 1, out);

    // 分岐 1: アーク i -> j のみ
    state.current_arcs.push_back(std::make_pair(i, j));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // 分岐 2: アーク j -> i のみ
    state.current_arcs.push_back(std::make_pair(j, i));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // 分岐 3: アーク i -> j と j -> i の両方
    state.current_arcs.push_back(std::make_pair(i, j));
    state.current_arcs.push_back(std::make_pair(j, i));
    digraph_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();
    state.current_arcs.pop_back();
}

} // namespace detail_digraph_enum

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き有向グラフを全列挙する
 * @param n 頂点数
 * @return DigraphEnumerationResult
 *
 * n(n-1)/2 個の非順序ペアに対し、各ペアの辺選択を 4 通りから選択する
 * DFS で全 4^(n(n-1)/2) = 2^(n(n-1)) 個のラベル付き有向グラフを構成する。
 */
inline DigraphEnumerationResult enumerate_digraphs(int n) {
    DigraphEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        DigraphEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_digraph_enum::DigraphEnumState state =
        detail_digraph_enum::digraph_build_state(n);
    detail_digraph_enum::digraph_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
