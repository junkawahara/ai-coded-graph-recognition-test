#ifndef GRAPH_RECOGNITION_APEX_H
#define GRAPH_RECOGNITION_APEX_H

/**
 * @file apex.h
 * @brief Apex グラフ認識
 *
 * Apex グラフとは、ある頂点を除去すると平面グラフになるグラフである。
 * 各頂点 v を除去して平面性を検査する O(n(n+m)) アルゴリズム。
 * マイナー閉 (遺伝的) クラスのため、誘導部分グラフに対して閉じている。
 */

#include <vector>

#include "graph.h"
#include "planar.h"

namespace graph_recognition {

/**
 * @brief Apex 認識アルゴリズムの選択
 */
enum class ApexAlgorithm {
    VERTEX_DELETION /**< 各頂点を除去して平面性テスト */
};

/**
 * @brief Apex 認識の結果
 */
struct ApexResult {
    bool is_apex = false; /**< Apex グラフであれば true */
};

/**
 * @brief グラフが Apex グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: VERTEX_DELETION)
 * @return ApexResult
 *
 * 各頂点 v を除去して G-v が平面グラフかどうかを判定する。
 * 1 つでも平面になれば apex。n <= 5 では全グラフが apex。
 */
inline ApexResult check_apex(const Graph& g,
    ApexAlgorithm algo = ApexAlgorithm::VERTEX_DELETION) {
    ApexResult res;
    res.is_apex = false;
    (void)algo;

    int n = g.n;
    if (n <= 5) {
        res.is_apex = true;
        return res;
    }

    for (int v = 1; v <= n; ++v) {
        // G-v を構築: 頂点 v を除外してリナンバリング
        std::vector<int> id(n + 1, 0);
        int cnt = 0;
        for (int u = 1; u <= n; ++u) {
            if (u != v) {
                id[u] = ++cnt;
            }
        }

        std::vector<std::pair<int, int> > edges;
        for (int u = 1; u <= n; ++u) {
            if (u == v) continue;
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (w == v) continue;
                if (id[w] > id[u]) {
                    edges.push_back(std::make_pair(id[u], id[w]));
                }
            }
        }

        Graph sg(cnt, edges);
        PlanarResult pr = check_planar(sg);
        if (pr.is_planar) {
            res.is_apex = true;
            return res;
        }
    }

    return res;
}

} // namespace graph_recognition

#endif
