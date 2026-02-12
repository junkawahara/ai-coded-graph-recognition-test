#ifndef GRAPH_RECOGNITION_AT_FREE_H
#define GRAPH_RECOGNITION_AT_FREE_H

/**
 * @file at_free.h
 * @brief AT-free グラフ認識
 *
 * 小惑星三つ組 (asteroidal triple) が存在しなければ AT-free と判定する。
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief AT-free グラフ認識アルゴリズムの選択
 */
enum class ATFreeAlgorithm {
    BRUTE_FORCE /**< 全三つ組の探索 */
};

/**
 * @brief AT-free グラフ認識の結果
 */
struct ATFreeResult {
    bool is_at_free; /**< AT-free であれば true */
};

namespace detail {

/**
 * @brief グラフに小惑星三つ組 (AT) が存在するか判定する
 * @param g 入力グラフ
 * @return AT が存在すれば true
 */
inline bool has_asteroidal_triple(const Graph& g) {
    int n = g.n;
    if (n < 3) return false;

    std::vector<std::vector<int>> comp(n + 1, std::vector<int>(n + 1, -1));

    for (int v = 1; v <= n; ++v) {
        std::vector<unsigned char> blocked(n + 1, 0);
        blocked[v] = 1;
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            blocked[g.adj[v][i]] = 1;
        }

        int label = 0;
        std::queue<int> q;
        for (int u = 1; u <= n; ++u) {
            if (blocked[u]) continue;
            if (comp[v][u] >= 0) continue;
            comp[v][u] = label;
            q.push(u);
            while (!q.empty()) {
                int cur = q.front();
                q.pop();
                for (size_t i = 0; i < g.adj[cur].size(); ++i) {
                    int w = g.adj[cur][i];
                    if (blocked[w]) continue;
                    if (comp[v][w] >= 0) continue;
                    comp[v][w] = label;
                    q.push(w);
                }
            }
            label++;
        }
    }

    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                if (comp[c][a] >= 0 && comp[c][b] >= 0 && comp[c][a] == comp[c][b] &&
                    comp[b][a] >= 0 && comp[b][c] >= 0 && comp[b][a] == comp[b][c] &&
                    comp[a][b] >= 0 && comp[a][c] >= 0 && comp[a][b] == comp[a][c]) {
                    return true;
                }
            }
        }
    }

    return false;
}

} // namespace detail

/**
 * @brief グラフが AT-free か判定する
 * @param g 入力グラフ
 * @return ATFreeResult
 *
 * 小惑星三つ組 (asteroidal triple) が存在しなければ AT-free。
 */
inline ATFreeResult check_at_free(const Graph& g,
    ATFreeAlgorithm algo = ATFreeAlgorithm::BRUTE_FORCE) {
    (void)algo;
    ATFreeResult res;
    res.is_at_free = !detail::has_asteroidal_triple(g);
    return res;
}

} // namespace graph_recognition

#endif
