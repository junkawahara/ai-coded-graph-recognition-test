#ifndef GRAPH_RECOGNITION_CLUSTER_H
#define GRAPH_RECOGNITION_CLUSTER_H

/**
 * @file cluster.h
 * @brief Cluster グラフ (P3-free) 認識
 *
 * Cluster グラフはクリークの非交和であり、誘導 P3 を含まないグラフと等価。
 * 各連結成分が完全グラフかどうかを検査して判定する。O(n+m) 時間。
 */

#include "graph.h"

#include <vector>

namespace graph_recognition {

/**
 * @brief Cluster グラフ認識アルゴリズムの選択
 */
enum class ClusterAlgorithm {
    COMPONENT_CLIQUE /**< 各連結成分がクリークか検査 */
};

/**
 * @brief Cluster グラフ認識の結果
 */
struct ClusterResult {
    bool is_cluster = false; /**< cluster グラフであれば true */
};

namespace detail_cluster {

/**
 * @brief 連結成分を BFS で取得する (内部関数)
 */
inline std::vector<std::vector<int> > find_components(const Graph& g) {
    std::vector<std::vector<int> > components;
    std::vector<char> visited(g.n + 1, 0);
    for (int s = 1; s <= g.n; ++s) {
        if (visited[s]) continue;
        std::vector<int> comp;
        std::vector<int> queue;
        queue.push_back(s);
        visited[s] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            comp.push_back(u);
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int v = g.adj[u][j];
                if (!visited[v]) {
                    visited[v] = 1;
                    queue.push_back(v);
                }
            }
        }
        components.push_back(comp);
    }
    return components;
}

} // namespace detail_cluster

/**
 * @brief グラフが cluster グラフか判定する
 * @param g 入力グラフ
 * @return ClusterResult
 *
 * G が cluster ⟺ 各連結成分が完全グラフ。
 * 成分サイズ k の完全グラフは辺数 k(k-1)/2 を持つ。
 */
inline ClusterResult check_cluster(const Graph& g,
    ClusterAlgorithm algo = ClusterAlgorithm::COMPONENT_CLIQUE) {
    (void)algo;
    ClusterResult res;
    res.is_cluster = false;

    std::vector<std::vector<int> > components =
        detail_cluster::find_components(g);

    for (size_t c = 0; c < components.size(); ++c) {
        int k = static_cast<int>(components[c].size());
        // 成分内の辺数をカウント
        long long edge_count = 0;
        for (size_t i = 0; i < components[c].size(); ++i) {
            int u = components[c][i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                ++edge_count;
            }
        }
        edge_count /= 2; // 各辺を 2 回カウントしたため
        long long expected = (long long)k * (k - 1) / 2;
        if (edge_count != expected) return res;
    }

    res.is_cluster = true;
    return res;
}

} // namespace graph_recognition

#endif
