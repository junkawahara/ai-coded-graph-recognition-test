#ifndef GRAPH_RECOGNITION_FOREST_H
#define GRAPH_RECOGNITION_FOREST_H

/**
 * @file forest.h
 * @brief 森 (forest) 認識
 *
 * 閉路の非存在を辺数と連結成分数の関係で判定する。
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief 森認識アルゴリズムの選択
 */
enum class ForestAlgorithm {
    BFS /**< BFS による連結成分列挙 + 辺数チェック */
};

/**
 * @brief 森認識の結果
 */
struct ForestResult {
    bool is_forest = false;    /**< 森であれば true */
    int num_components = 0;    /**< 連結成分数 */
};

/**
 * @brief グラフが森か判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: BFS)
 * @return ForestResult
 *
 * 森 ⟺ m = n - (連結成分数)。各成分が木。
 */
inline ForestResult check_forest(const Graph& g,
    ForestAlgorithm algo = ForestAlgorithm::BFS) {
    (void)algo;
    ForestResult res;

    int n = g.n;
    if (n == 0) {
        res.is_forest = true;
        return res;
    }

    /* 連結成分数を数える */
    std::vector<char> visited(n + 1, 0);
    int components = 0;
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;
        ++components;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
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
    }

    /* 森 ⟺ m = n - components */
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    res.num_components = components;
    if (m == (long long)n - components) {
        res.is_forest = true;
    }
    return res;
}

} // namespace graph_recognition

#endif
