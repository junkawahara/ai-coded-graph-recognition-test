#ifndef GRAPH_RECOGNITION_PLANAR_H
#define GRAPH_RECOGNITION_PLANAR_H

/**
 * @file planar.h
 * @brief 平面グラフ (planar graph) 認識
 *
 * de Fraysseix-Rosenstiehl のインターレースメント法 (O(n^2)) で判定する。
 * 連結成分ごとに DFS 木を構築し、バックエッジのインターレースメントグラフが
 * 二部グラフであるかを検査する。
 *
 * フォールバックとして Kuratowski の定理に基づくマイナーチェックも利用可能。
 */

#include "graph.h"
#include "minor.h"

#include <algorithm>
#include <queue>
#include <vector>

namespace graph_recognition {

/**
 * @brief 平面グラフ認識アルゴリズムの選択
 */
enum class PlanarAlgorithm {
    LR_INTERLACEMENT, /**< de Fraysseix-Rosenstiehl インターレースメント法 */
    MINOR_CHECK       /**< マイナーチェック (指数時間) */
};

/**
 * @brief 平面グラフ認識の結果
 */
struct PlanarResult {
    bool is_planar; /**< 平面グラフであれば true */
};

namespace detail_planar {

/**
 * @brief バックエッジ (d -> a) を表す構造体。d は子孫、a は祖先。
 */
struct BackEdge {
    int d; /**< DFS 木における子孫側の端点 */
    int a; /**< DFS 木における祖先側の端点 */
};

/**
 * @brief 連結成分に対して de Fraysseix-Rosenstiehl のインターレースメント法を適用
 *
 * @param g 入力グラフ
 * @param component 連結成分の頂点リスト
 * @return true iff この連結成分が平面的
 */
inline bool check_component_planar(const Graph& g,
                                   const std::vector<int>& component) {
    int cn = static_cast<int>(component.size());
    if (cn <= 4) return true;

    // 辺数カウント
    long long m = 0;
    for (size_t i = 0; i < component.size(); ++i) {
        m += static_cast<long long>(g.adj[component[i]].size());
    }
    m /= 2;
    if (m > 3LL * cn - 6) return false;

    // --- 反復 DFS で木を構築 ---
    int root = component[0];
    std::vector<int> entry(g.n + 1, 0);
    std::vector<int> exit_time(g.n + 1, 0);
    std::vector<int> parent(g.n + 1, 0);
    int timer = 0;

    // DFS スタック: (vertex, adj_index, entering?)
    // entering=true のとき entry を記録、false のとき exit を記録
    struct DfsFrame {
        int v;
        size_t idx;
    };
    std::vector<DfsFrame> stk;
    stk.reserve(cn);

    entry[root] = ++timer;
    parent[root] = -1;
    DfsFrame f0;
    f0.v = root;
    f0.idx = 0;
    stk.push_back(f0);

    std::vector<BackEdge> back_edges;

    while (!stk.empty()) {
        DfsFrame& cur = stk.back();
        int v = cur.v;
        if (cur.idx < g.adj[v].size()) {
            int w = g.adj[v][cur.idx];
            ++cur.idx;
            if (entry[w] == 0) {
                // tree edge
                parent[w] = v;
                entry[w] = ++timer;
                DfsFrame nf;
                nf.v = w;
                nf.idx = 0;
                stk.push_back(nf);
            } else if (w != parent[v] && entry[w] < entry[v]) {
                // back edge: v (descendant) -> w (ancestor)
                BackEdge be;
                be.d = v;
                be.a = w;
                back_edges.push_back(be);
            }
        } else {
            exit_time[v] = ++timer;
            stk.pop_back();
        }
    }

    // --- 祖先関係の判定 ---
    // is_ancestor(u, v): u が v の祖先か (u == v も含む)
    // entry[u] <= entry[v] && exit_time[v] <= exit_time[u]
    // is_strict_ancestor: さらに u != v

    // --- インターレースメント判定 ---
    // バックエッジ e1=(d1->a1) と e2=(d2->a2) がインターレースする条件:
    // Case 1: a1 は a2 の真の祖先、a2 は d1 の真の祖先、d1 は d2 の真の祖先
    // Case 2: a2 は a1 の真の祖先、a1 は d2 の真の祖先、d2 は d1 の真の祖先
    // (対称)

    int nb = static_cast<int>(back_edges.size());
    if (nb == 0) return true; // tree is always planar

    // インターレースメントグラフを構築し、二部グラフかどうか BFS で判定
    // back_edges を 0-indexed で扱う
    std::vector<std::vector<int>> ig(nb); // interlacement graph adjacency

    for (int i = 0; i < nb; ++i) {
        int d1 = back_edges[i].d;
        int a1 = back_edges[i].a;
        for (int j = i + 1; j < nb; ++j) {
            int d2 = back_edges[j].d;
            int a2 = back_edges[j].a;

            // Case 1: a1 strict ancestor of a2, a2 strict ancestor of d1,
            //         d1 strict ancestor of d2
            bool case1 = false;
            {
                // a1 strict ancestor of a2
                bool c1 = (entry[a1] < entry[a2]) &&
                           (exit_time[a2] < exit_time[a1]);
                // a2 strict ancestor of d1
                bool c2 = (entry[a2] < entry[d1]) &&
                           (exit_time[d1] < exit_time[a2]);
                // d1 strict ancestor of d2
                bool c3 = (entry[d1] < entry[d2]) &&
                           (exit_time[d2] < exit_time[d1]);
                case1 = c1 && c2 && c3;
            }

            // Case 2: symmetric (swap roles of e1 and e2)
            bool case2 = false;
            {
                // a2 strict ancestor of a1
                bool c1 = (entry[a2] < entry[a1]) &&
                           (exit_time[a1] < exit_time[a2]);
                // a1 strict ancestor of d2
                bool c2 = (entry[a1] < entry[d2]) &&
                           (exit_time[d2] < exit_time[a1]);
                // d2 strict ancestor of d1
                bool c3 = (entry[d2] < entry[d1]) &&
                           (exit_time[d1] < exit_time[d2]);
                case2 = c1 && c2 && c3;
            }

            if (case1 || case2) {
                ig[i].push_back(j);
                ig[j].push_back(i);
            }
        }
    }

    // --- BFS 二部グラフ判定 ---
    std::vector<int> color(nb, -1);
    for (int s = 0; s < nb; ++s) {
        if (color[s] != -1) continue;
        color[s] = 0;
        std::queue<int> q;
        q.push(s);
        while (!q.empty()) {
            int u = q.front();
            q.pop();
            for (size_t k = 0; k < ig[u].size(); ++k) {
                int w = ig[u][k];
                if (color[w] == -1) {
                    color[w] = 1 - color[u];
                    q.push(w);
                } else if (color[w] == color[u]) {
                    return false; // odd cycle -> not bipartite -> not planar
                }
            }
        }
    }

    return true;
}

} // namespace detail_planar

/**
 * @brief グラフが平面グラフか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: LR_INTERLACEMENT)
 * @return PlanarResult
 */
inline PlanarResult check_planar(const Graph& g,
    PlanarAlgorithm algo = PlanarAlgorithm::LR_INTERLACEMENT) {
    PlanarResult res;
    res.is_planar = false;

    int n = g.n;
    long long m = 0;
    for (int v = 1; v <= n; ++v) m += (long long)g.adj[v].size();
    m /= 2;

    if (n <= 4) {
        res.is_planar = true;
        return res;
    }

    // 単純平面グラフの辺数上界。
    if (n >= 3 && m > 3LL * n - 6) return res;

    if (algo == PlanarAlgorithm::MINOR_CHECK) {
        detail_minor::MinorState st = detail_minor::build_minor_state(g);

        detail_minor::MinorChecker k5_checker(detail_minor::MinorTarget::K5);
        if (k5_checker.has_minor(st)) return res;

        detail_minor::MinorChecker k33_checker(detail_minor::MinorTarget::K33);
        if (k33_checker.has_minor(st)) return res;

        res.is_planar = true;
        return res;
    }

    // LR_INTERLACEMENT: 連結成分ごとに判定
    std::vector<bool> visited(n + 1, false);
    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;
        // BFS で連結成分を収集
        std::vector<int> component;
        std::queue<int> q;
        q.push(s);
        visited[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            component.push_back(v);
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int w = g.adj[v][i];
                if (!visited[w]) {
                    visited[w] = true;
                    q.push(w);
                }
            }
        }
        if (!detail_planar::check_component_planar(g, component)) {
            return res; // not planar
        }
    }

    res.is_planar = true;
    return res;
}

} // namespace graph_recognition

#endif
