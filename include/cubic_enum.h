#ifndef GRAPH_RECOGNITION_CUBIC_ENUM_H
#define GRAPH_RECOGNITION_CUBIC_ENUM_H

/**
 * @file cubic_enum.h
 * @brief 三次グラフ (3-正則グラフ) の列挙 (逆探索)
 *
 * 頂点集合 {1, ..., n} 上のラベル付き三次グラフを全列挙する。
 *
 * 三次グラフ: 全頂点の次数がちょうど 3 のグラフ。
 * n が奇数または n < 4 の場合、三次グラフは存在しない。
 *
 * アルゴリズム:
 *   頂点を 1, 2, ..., n の順に追加。各頂点 x の追加時に
 *   {1,...,x-1} の中で deg < 3 の頂点から近傍を選択。
 *   次数上限・到達可能性で枝刈りし、
 *   全頂点追加後に全次数 == 3 を確認して出力。
 *
 * 参考文献:
 *   Brinkmann, Goedgebeur, McKay, "Generation of Cubic graphs,"
 *   J. Graph Theory 86, 2017
 *   Meringer, "Fast Generation of Regular Graphs and Construction of Cages,"
 *   J. Graph Theory 30, 1999, pp. 137-146
 */

#include <cstddef>
#include <utility>
#include <vector>

#include "chordal_enum.h"

namespace graph_recognition {

enum class CubicEnumAlgorithm {
    REVERSE_SEARCH
};

struct CubicEnumerationResult {
    std::vector<EnumeratedGraph> graphs;
};

namespace detail {

struct CubicEnumState {
    int total_n;
    int alive_count;
    std::vector<std::vector<char> > adj;
    std::vector<int> deg;

    explicit CubicEnumState(int n)
        : total_n(n), alive_count(0),
          adj(n + 1, std::vector<char>(n + 1, 0)),
          deg(n + 1, 0) {}
};

inline void cubic_enum_choose(CubicEnumState& state,
                               const std::vector<int>& available,
                               std::size_t start,
                               int chosen_count,
                               int min_size, int max_size,
                               std::vector<EnumeratedGraph>* out);

inline void cubic_enum_dfs(CubicEnumState& state,
                            std::vector<EnumeratedGraph>* out) {
    if (state.alive_count == state.total_n) {
        for (int v = 1; v <= state.total_n; ++v) {
            if (state.deg[v] != 3) return;
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
    int remaining = state.total_n - x;

    std::vector<int> available;
    for (int v = 1; v < x; ++v) {
        if (state.deg[v] < 3) {
            available.push_back(v);
        }
    }

    int min_neighbors = 3 - remaining;
    if (min_neighbors < 0) min_neighbors = 0;
    int max_neighbors = 3;
    if (max_neighbors > (int)available.size()) max_neighbors = (int)available.size();

    if (max_neighbors < min_neighbors) return;

    cubic_enum_choose(state, available, 0, 0,
                       min_neighbors, max_neighbors, out);
}

inline void cubic_enum_choose(CubicEnumState& state,
                               const std::vector<int>& available,
                               std::size_t start,
                               int chosen_count,
                               int min_size, int max_size,
                               std::vector<EnumeratedGraph>* out) {
    int x = state.alive_count + 1;
    int remaining_after_x = state.total_n - x;

    if (chosen_count >= min_size) {
        state.deg[x] = chosen_count;
        state.alive_count = x;

        bool feasible = true;
        for (int v = 1; v <= x; ++v) {
            if (state.deg[v] + remaining_after_x < 3) {
                feasible = false;
                break;
            }
        }

        if (feasible) {
            cubic_enum_dfs(state, out);
        }

        state.alive_count = x - 1;
        state.deg[x] = 0;
    }

    if (chosen_count == max_size) return;

    int can_still_choose = (int)available.size() - (int)start;
    if (chosen_count + can_still_choose < min_size) return;

    for (std::size_t i = start; i < available.size(); ++i) {
        int v = available[i];
        if (state.deg[v] >= 3) continue;

        state.adj[x][v] = 1;
        state.adj[v][x] = 1;
        state.deg[v]++;

        cubic_enum_choose(state, available, i + 1, chosen_count + 1,
                           min_size, max_size, out);

        state.adj[x][v] = 0;
        state.adj[v][x] = 0;
        state.deg[v]--;
    }
}

}  // namespace detail

/**
 * @brief 頂点集合 {1, ..., n} 上のラベル付き三次グラフを全列挙する
 * @param n 頂点数
 * @param algo アルゴリズム選択 (現在は REVERSE_SEARCH のみ)
 * @return CubicEnumerationResult
 */
inline CubicEnumerationResult
enumerate_cubic_graphs(int n,
    CubicEnumAlgorithm algo = CubicEnumAlgorithm::REVERSE_SEARCH) {
    (void)algo;
    CubicEnumerationResult result;
    if (n <= 0) return result;
    if (n % 2 != 0) return result;
    if (n < 4) return result;

    detail::CubicEnumState root(n);
    detail::cubic_enum_dfs(root, &result.graphs);
    return result;
}

}  // namespace graph_recognition

#endif
