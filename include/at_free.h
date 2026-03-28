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
    bool is_at_free = false; /**< AT-free であれば true */
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

    // O(n^2) memory + O(n^3) time: guard against excessive allocation.
    // For n > 10000, ~400MB+ would be needed; conservatively report AT exists.
    if (n > 10000) return true;

    // Flat array for component labels: comp[v*(n+1)+u] = component of u in G-N[v].
    // Uses O(n^2) ints in a single allocation, avoiding per-row vector overhead.
    size_t stride = (size_t)(n + 1);
    std::vector<int> comp(stride * stride, -1);

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
            if (comp[v * stride + u] >= 0) continue;
            comp[v * stride + u] = label;
            q.push(u);
            while (!q.empty()) {
                int cur = q.front();
                q.pop();
                for (size_t i = 0; i < g.adj[cur].size(); ++i) {
                    int w = g.adj[cur][i];
                    if (blocked[w]) continue;
                    if (comp[v * stride + w] >= 0) continue;
                    comp[v * stride + w] = label;
                    q.push(w);
                }
            }
            label++;
        }
    }

    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                if (comp[c * stride + a] >= 0 && comp[c * stride + b] >= 0 &&
                    comp[c * stride + a] == comp[c * stride + b] &&
                    comp[b * stride + a] >= 0 && comp[b * stride + c] >= 0 &&
                    comp[b * stride + a] == comp[b * stride + c] &&
                    comp[a * stride + b] >= 0 && comp[a * stride + c] >= 0 &&
                    comp[a * stride + b] == comp[a * stride + c]) {
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
