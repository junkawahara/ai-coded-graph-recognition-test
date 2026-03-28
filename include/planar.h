#ifndef GRAPH_RECOGNITION_PLANAR_H
#define GRAPH_RECOGNITION_PLANAR_H

/**
 * @file planar.h
 * @brief 平面グラフ (planar graph) 認識
 *
 * Kuratowski の定理に基づき K5 および K3,3 マイナーの非存在を
 * 確認する。辺数上界 m ≤ 3n-6 による高速フィルタ付き。
 */

#include "graph.h"
#include "minor.h"

#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 平面グラフ認識アルゴリズムの選択
 */
enum class PlanarAlgorithm {
    MINOR_CHECK  /**< K5/K3,3 マイナーチェック */
};

/**
 * @brief 平面グラフ認識の結果
 */
struct PlanarResult {
    bool is_planar = false; /**< 平面グラフであれば true */
};

/**
 * @brief グラフが平面グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: MINOR_CHECK)
 * @return PlanarResult
 */
inline PlanarResult check_planar(const Graph& g,
    PlanarAlgorithm algo = PlanarAlgorithm::MINOR_CHECK) {
    PlanarResult res;
    res.is_planar = false;
    (void)algo;

    int n = g.n;
    if (n <= 4) {
        res.is_planar = true;
        return res;
    }

    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    // 単純平面グラフの辺数上界
    if (n >= 3 && m > 3LL * n - 6) return res;

    // 連結成分ごとに K5/K3,3 マイナーを検査
    std::vector<bool> visited(n + 1, false);
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;

        // BFS で連結成分を収集
        std::vector<int> comp;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            comp.push_back(v);
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int w = g.adj[v][i];
                if (!visited[w]) {
                    visited[w] = true;
                    q.push(w);
                }
            }
        }

        int cn = static_cast<int>(comp.size());
        if (cn <= 4) continue;

        // 成分の辺数チェック
        long long cm = 0;
        for (size_t i = 0; i < comp.size(); ++i)
            cm += (long long)g.adj[comp[i]].size();
        cm /= 2;
        if (cm > 3LL * cn - 6) return res;

        // 成分の部分グラフを構築してマイナーチェック
        // 頂点を 1..cn にリナンバリング
        std::vector<int> id(n + 1, 0);
        for (int i = 0; i < cn; ++i) id[comp[i]] = i + 1;

        std::vector<std::pair<int, int> > edges;
        for (int i = 0; i < cn; ++i) {
            int u = comp[i];
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int v = g.adj[u][j];
                if (id[v] > id[u]) {
                    edges.push_back(std::make_pair(id[u], id[v]));
                }
            }
        }

        Graph sg(cn, edges);
        detail_minor::MinorState st = detail_minor::build_minor_state(sg);

        detail_minor::MinorChecker k5(detail_minor::MinorTarget::K5);
        if (k5.has_minor(st)) return res;

        detail_minor::MinorChecker k33(detail_minor::MinorTarget::K33);
        if (k33.has_minor(st)) return res;

        // リナンバリングをクリア
        for (int i = 0; i < cn; ++i) id[comp[i]] = 0;
    }

    res.is_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
