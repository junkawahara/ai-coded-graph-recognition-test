#ifndef GRAPH_RECOGNITION_UNICYCLIC_H
#define GRAPH_RECOGNITION_UNICYCLIC_H

/**
 * @file unicyclic.h
 * @brief 一閉路グラフ (unicyclic graph) 認識
 *
 * 連結かつ辺数 = 頂点数で判定する。
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief 一閉路グラフ認識アルゴリズムの選択
 */
enum class UnicyclicAlgorithm {
    BFS /**< BFS による連結性 + 辺数チェック */
};

/**
 * @brief 一閉路グラフ認識の結果
 */
struct UnicyclicResult {
    bool is_unicyclic = false; /**< 一閉路グラフであれば true */
};

/**
 * @brief グラフが一閉路グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: BFS)
 * @return UnicyclicResult
 *
 * 一閉路グラフ ⟺ 連結かつ m = n。
 */
inline UnicyclicResult check_unicyclic(const Graph& g,
    UnicyclicAlgorithm algo = UnicyclicAlgorithm::BFS) {
    (void)algo;
    UnicyclicResult res;

    int n = g.n;
    if (n == 0) return res;

    /* 辺数チェック */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;
    if (m != (long long)n) return res;

    /* BFS で連結性チェック */
    std::vector<char> visited(n + 1, 0);
    std::vector<int> queue;
    queue.push_back(1);
    visited[1] = 1;
    for (size_t qi = 0; qi < queue.size(); ++qi) {
        int v = queue[qi];
        for (size_t i = 0; i < g.adj[v].size(); ++i) {
            int u = g.adj[v][i];
            if (!visited[u]) {
                visited[u] = 1;
                queue.push_back(u);
            }
        }
    }
    if ((int)queue.size() != n) return res;

    res.is_unicyclic = true;
    return res;
}

} // namespace graph_recognition

#endif
