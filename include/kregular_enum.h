#ifndef GRAPH_RECOGNITION_KREGULAR_ENUM_H
#define GRAPH_RECOGNITION_KREGULAR_ENUM_H

/**
 * @file kregular_enum.h
 * @brief k-正則グラフの列挙 (逆探索)
 *
 * 頂点集合 {1, ..., n} 上のラベル付き k-正則グラフを全列挙する。
 *
 * k-正則グラフ: 全頂点の次数がちょうど k のグラフ。
 * 遺伝的クラスではないため、次数制約による枝刈りで探索空間を削減する。
 *
 * アルゴリズム:
 *   頂点を 1, 2, ..., n の順に追加。各頂点 x の追加時に
 *   {1,...,x-1} の中で deg < k の頂点から近傍を選択。
 *   次数上限・到達可能性・偶奇制約で枝刈りし、
 *   全頂点追加後に全次数 == k を確認して出力。
 *
 * 参考文献:
 *   Meringer, "Fast Generation of Regular Graphs and Construction of Cages,"
 *   J. Graph Theory 30, 1999, pp. 137-146
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

/**
 * @brief k-正則列挙アルゴリズムの選択
 */
enum class KRegularEnumAlgorithm {
    REVERSE_SEARCH /**< 逆探索 (次数制約付き) */
};

/**
 * @brief k-正則列挙の結果
 */
struct KRegularEnumerationResult {
    std::vector<EnumeratedGraph> graphs; /**< 列挙された k-正則グラフの配列 */
};

namespace detail {

/** @brief 逆探索の内部状態 */
struct KRegularEnumState {
    int total_n;
    int target_k;
    int alive_count;  /**< 存在する頂点は {1, ..., alive_count} */
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;  /**< 各頂点の現在の次数 */

    KRegularEnumState(int n, int k)
        : total_n(n), target_k(k), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

/**
 * @brief 部分集合列挙 + 再帰の内部関数
 *
 * available[start..] から近傍を選び、サイズが [min_size, max_size] の
 * 部分集合について再帰を試みる。
 */
inline void kregular_enum_choose(KRegularEnumState& state,
                                  const std::vector<int>& available,
                                  std::size_t start,
                                  int chosen_count,
                                  int min_size, int max_size,
                                  std::vector<EnumeratedGraph>* out);

/**
 * @brief k-正則逆探索の主 DFS
 */
inline void kregular_enum_dfs(KRegularEnumState& state,
                               std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        // 全頂点追加済み: 全次数 == k か確認
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.deg[v] != state.target_k) return;
        }
        EnumeratedGraph graph;
        graph.n = state.total_n;
        for (int u = 1; u <= state.total_n; ++u)
            for (int v = u + 1; v <= state.total_n; ++v)
                if (state.adj[u][v])
                    graph.edges.push_back(std::make_pair(u, v));
        out->push_back(graph);
        return;
    }

    int x = state.alive_count + 1;
    int k = state.target_k;
    int remaining = state.total_n - x;  // vertices after x

    // 候補: {1,...,x-1} の中で deg < k の頂点
    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < k) {
            available.push_back(v);
        }
    }

    // x の近傍数の範囲
    // x は将来 remaining 個の頂点から辺を得られる
    // よって今選ぶ近傍数 + remaining >= k, つまり min = max(0, k - remaining)
    // また max = min(k, |available|)
    int min_neighbors = k - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = k;
    if (max_neighbors > (int)available.size()) max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    kregular_enum_choose(state, available, 0, 0,
                          min_neighbors, max_neighbors, out);
}

inline void kregular_enum_choose(KRegularEnumState& state,
                                  const std::vector<int>& available,
                                  std::size_t start,
                                  int chosen_count,
                                  int min_size, int max_size,
                                  std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_n - x;

    if (chosen_count >= min_size) {
        // この部分集合で試行
        state.deg[x] = chosen_count;
        state.alive_count = x;

        // 到達可能性チェック: 全頂点 v in {1,...,x} で
        // deg[v] + (n - x) >= k が必要
        bool feasible = true;
        for (int v = 1; v <= x; ++v) {
            if (state.deg[v] + remaining_after_x < state.target_k) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            kregular_enum_dfs(state, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    // 残り候補数で枝刈り
    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        // v の次数上限チェック (choose 内で既に deg < k だが念のため)
        if (state.deg[v] >= state.target_k) continue;

        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;

        kregular_enum_choose(state, available, i + 1, chosen_count + 1,
                              min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き k-正則グラフを全列挙する
 * @param n 頂点数
 * @param k 目標次数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return KRegularEnumerationResult
 */
inline KRegularEnumerationResult
enumerate_kregular_graphs_reverse_search(int n, int k,
    KRegularEnumAlgorithm algo =
        KRegularEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    KRegularEnumerationResult result;
    if (n <= 0) {
        if (n == 0 && k == 0) {
            // 0 頂点 0-正則: 空グラフ 1 つ
            EnumeratedGraph g;
            g.n = 0;
            result.graphs.push_back(g);
        }
        return result;
    }
    // 握手補題: n*k は偶数でなければならない
    if ((long long)n * k % 2 != 0) return result;
    // k >= n なら不可能 (最大次数は n-1)
    if (k >= n) return result;

    detail::KRegularEnumState root(n, k);
    detail::kregular_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
