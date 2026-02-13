#ifndef GRAPH_RECOGNITION_GRAPH_H
#define GRAPH_RECOGNITION_GRAPH_H

/**
 * @file graph.h
 * @brief グラフの基本データ構造
 *
 * 1-indexed の無向グラフを隣接リストと隣接セットで表現する。
 */

#include <iostream>
#include <unordered_set>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief 1-indexed 無向グラフ (隣接リスト + 隣接セット)
 *
 * 頂点番号は 1 から n まで。隣接リスト adj と隣接セット adj_set の
 * 両方を保持し、走査と存在判定を効率的に行う。
 */
struct Graph {
    int n; /**< 頂点数 */
    std::vector<std::vector<int>> adj;            /**< 隣接リスト (1-indexed) */
    std::vector<std::unordered_set<int>> adj_set; /**< 隣接セット (1-indexed) */

    /** @brief デフォルトコンストラクタ (空グラフ) */
    Graph() : n(0), adj(1), adj_set(1) {}

    /**
     * @brief 頂点数と辺リストからグラフを構築する
     * @param n 頂点数
     * @param edges 辺リスト (1-indexed の頂点ペア)
     *
     * 自己ループや多重辺は自動的に無視される。
     */
    Graph(int n, const std::vector<std::pair<int, int>>& edges)
        : n(n), adj(n + 1), adj_set(n + 1) {
        for (size_t i = 0; i < edges.size(); ++i) {
            int u = edges[i].first, v = edges[i].second;
            if (u < 1 || u > n || v < 1 || v > n) continue;
            if (u == v) continue;
            if (adj_set[u].count(v)) continue;
            adj_set[u].insert(v);
            adj_set[v].insert(u);
            adj[u].push_back(v);
            adj[v].push_back(u);
        }
    }

    /**
     * @brief 辺の存在判定
     * @param u 頂点 u
     * @param v 頂点 v
     * @return 辺 (u, v) が存在すれば true
     */
    bool has_edge(int u, int v) const {
        return adj_set[u].count(v) > 0;
    }

    /**
     * @brief 入力ストリームからグラフを読み込む
     * @param in 入力ストリーム
     * @return 読み込んだグラフ
     *
     * 入力形式: 1行目に n m、続く m 行に辺 u v。
     */
    static Graph read(std::istream& in) {
        int n, m;
        if (!(in >> n >> m)) return Graph();
        std::vector<std::pair<int, int>> edges;
        edges.reserve(m);
        for (int i = 0; i < m; ++i) {
            int u, v;
            if (!(in >> u >> v)) break;
            edges.push_back(std::make_pair(u, v));
        }
        return Graph(n, edges);
    }
};

} // namespace graph_recognition

#endif
