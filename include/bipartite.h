#ifndef GRAPH_RECOGNITION_BIPARTITE_H
#define GRAPH_RECOGNITION_BIPARTITE_H

/**
 * @file bipartite.h
 * @brief 二部グラフ (bipartite graph) 認識
 *
 * BFS による 2-彩色を用いて二部グラフを判定する。
 */

#include "graph.h"
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 二部グラフ認識アルゴリズムの選択
 */
enum class BipartiteAlgorithm {
    BFS /**< BFS による 2-彩色 */
};

/**
 * @brief 二部グラフ認識の結果
 */
struct BipartiteResult {
    bool is_bipartite;          /**< 二部グラフであれば true */
    std::vector<int> color;     /**< color[v]: 頂点 v の色 (0 または 1)。is_bipartite == true の場合のみ有効 */
};

/**
 * @brief グラフが二部グラフか判定する
 * @param g 入力グラフ
 * @return BipartiteResult
 *
 * BFS で 2-彩色を試み、矛盾が生じなければ二部グラフと判定する。
 */
inline BipartiteResult check_bipartite(const Graph& g,
    BipartiteAlgorithm algo = BipartiteAlgorithm::BFS) {
    (void)algo;
    BipartiteResult res;
    res.is_bipartite = true;
    res.color.assign(g.n + 1, -1);

    std::queue<int> q;
    for (int s = 1; s <= g.n; ++s) {
        if (res.color[s] != -1) continue;
        res.color[s] = 0;
        q.push(s);

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (res.color[u] == -1) {
                    res.color[u] = 1 - res.color[v];
                    q.push(u);
                    continue;
                }
                if (res.color[u] == res.color[v]) {
                    res.is_bipartite = false;
                    return res;
                }
            }
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
