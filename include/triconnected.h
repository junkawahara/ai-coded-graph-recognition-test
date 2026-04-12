#ifndef GRAPH_RECOGNITION_TRICONNECTED_H
#define GRAPH_RECOGNITION_TRICONNECTED_H

/**
 * @file triconnected.h
 * @brief 3-連結 (triconnected) グラフ認識
 *
 * グラフが 3-連結であるかを判定する。
 * 3-連結グラフとは、頂点数 >= 4 の連結グラフで、任意の 2 頂点を
 * 除去しても連結であるもの (頂点連結度 κ(G) >= 3)。
 *
 * アルゴリズム:
 *   - NAIVE: 全頂点ペア除去 + 連結性検査 O(n^2 * (n+m))
 */

#include "graph.h"
#include <vector>

namespace graph_recognition {

/**
 * @brief 3-連結認識アルゴリズムの選択
 */
enum class TriconnectedAlgorithm {
    NAIVE /**< 全頂点ペア除去 (デフォルト) */
};

/**
 * @brief 3-連結認識の結果
 */
struct TriconnectedResult {
    bool is_triconnected = false; /**< 3-連結グラフであれば true */
};

/**
 * @brief グラフが 3-連結か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: NAIVE)
 * @return TriconnectedResult
 *
 * 3-連結グラフ: 頂点数 >= 4、連結、任意の 2 頂点除去後も連結。
 * Naive: 全 O(n^2) 頂点ペアについて G-{u,v} の連結性を BFS で検査。
 */
inline TriconnectedResult check_triconnected(const Graph& g,
    TriconnectedAlgorithm algo = TriconnectedAlgorithm::NAIVE) {
    (void)algo;
    TriconnectedResult res;
    res.is_triconnected = false;

    int n = g.n;
    if (n < 4) return res;

    // 必要条件: 全頂点の次数 >= 3
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() < 3) return res;
    }

    // 全頂点ペア (u, v) について G-{u,v} の連結性を検査
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            // G-{u,v} が連結か BFS で検査
            // 開始頂点を u,v 以外から選ぶ
            int start = -1;
            for (int w = 1; w <= n; ++w) {
                if (w != u && w != v) {
                    start = w;
                    break;
                }
            }
            if (start == -1) return res;  // n-2 == 0, 不可能

            std::vector<char> visited(n + 1, 0);
            std::vector<int> queue;
            queue.push_back(start);
            visited[start] = 1;
            int cnt = 1;

            for (size_t qi = 0; qi < queue.size(); ++qi) {
                int w = queue[qi];
                for (size_t i = 0; i < g.adj[w].size(); ++i) {
                    int x = g.adj[w][i];
                    if (x != u && x != v && !visited[x]) {
                        visited[x] = 1;
                        queue.push_back(x);
                        cnt++;
                    }
                }
            }

            if (cnt != n - 2) return res;  // 非連結
        }
    }

    res.is_triconnected = true;
    return res;
}

}  // namespace graph_recognition

#endif
