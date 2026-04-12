#ifndef GRAPH_RECOGNITION_TOURNAMENT_ENUM_H
#define GRAPH_RECOGNITION_TOURNAMENT_ENUM_H

/**
 * @file tournament_enum.h
 * @brief Tournament (トーナメント) のラベル付き全列挙
 *
 * 完全グラフ K_n の全辺に向きを付けた有向グラフ (トーナメント) を
 * 構成的に全列挙する。n(n-1)/2 個の辺ペアそれぞれについて
 * 2 通りの向きを DFS で選択し、全 2^(n(n-1)/2) 個のラベル付き
 * トーナメントを重複なく列挙する。
 *
 * 構成の各段階で得られるグラフは定義上必ずトーナメントであるため、
 * 認識フィルタは不要 (構成的列挙)。
 *
 * 参考文献:
 *   - Harary, Palmer, "Graphical Enumeration," Academic Press, 1973
 *   - Moon, "Topics on Tournaments," Holt, Rinehart & Winston, 1968
 */

#include <algorithm>
#include <cstddef>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 列挙されたトーナメント (有向完全グラフ)
 */
struct TournamentEnumeratedGraph {
    int n;                                        /**< 頂点数 */
    std::vector<std::pair<int, int> > arcs;       /**< 有向辺リスト (u, v) = u->v, ソート済み */
};

/**
 * @brief Tournament 列挙の結果
 */
struct TournamentEnumerationResult {
    std::vector<TournamentEnumeratedGraph> graphs; /**< 列挙されたトーナメントの配列 */
};

namespace detail_tournament_enum {

/** @brief 構成的列挙の内部状態 */
struct TournamentEnumState {
    int n;                                         /**< 頂点数 */
    int num_pairs;                                 /**< C(n,2): 辺ペア数 */
    std::vector<std::pair<int, int> > pairs;       /**< K_n の全ペア (i<j, 辞書順) */
    std::vector<std::pair<int, int> > current_arcs; /**< 現在選択中のアーク列 */
};

/**
 * @brief 状態の初期化: 全辺ペアを構築
 */
inline TournamentEnumState tournament_build_state(int n) {
    TournamentEnumState state;
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
 * pair_idx 番目の辺ペア (i, j) について向き i->j / j->i の 2 分岐で探索。
 * 全ペアの向きが決定したらトーナメントを出力する。
 */
inline void tournament_enum_dfs(TournamentEnumState& state, int pair_idx,
                                std::vector<TournamentEnumeratedGraph>* out) {
    if (pair_idx == state.num_pairs) {
        TournamentEnumeratedGraph g;
        g.n = state.n;
        g.arcs = state.current_arcs;
        std::sort(g.arcs.begin(), g.arcs.end());
        out->push_back(g);
        return;
    }

    int i = state.pairs[pair_idx].first;
    int j = state.pairs[pair_idx].second;

    // 分岐 1: アーク i -> j
    state.current_arcs.push_back(std::make_pair(i, j));
    tournament_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();

    // 分岐 2: アーク j -> i
    state.current_arcs.push_back(std::make_pair(j, i));
    tournament_enum_dfs(state, pair_idx + 1, out);
    state.current_arcs.pop_back();
}

} // namespace detail_tournament_enum

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付きトーナメントを全列挙する
 * @param n 頂点数
 * @return TournamentEnumerationResult
 *
 * K_n の n(n-1)/2 個の辺ペアに対し、各ペアの向きを 2 通りから選択する
 * DFS で全 2^(n(n-1)/2) 個のラベル付きトーナメントを構成する。
 */
inline TournamentEnumerationResult enumerate_tournaments(int n) {
    TournamentEnumerationResult result;
    if (n <= 0) return result;
    if (n == 1) {
        TournamentEnumeratedGraph g;
        g.n = 1;
        result.graphs.push_back(g);
        return result;
    }

    detail_tournament_enum::TournamentEnumState state =
        detail_tournament_enum::tournament_build_state(n);
    detail_tournament_enum::tournament_enum_dfs(state, 0, &result.graphs);
    return result;
}

} // namespace graph_recognition

#endif
