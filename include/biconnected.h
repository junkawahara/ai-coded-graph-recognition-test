#ifndef GRAPH_RECOGNITION_BICONNECTED_H
#define GRAPH_RECOGNITION_BICONNECTED_H

/**
 * @file biconnected.h
 * @brief 2-連結 (biconnected) グラフ認識
 *
 * グラフが 2-連結であるかを判定する。
 * 2-連結グラフとは、頂点数 >= 3 の連結グラフで切断点を持たないもの。
 *
 * アルゴリズム:
 *   - DFS: Tarjan の切断点検出 O(n+m)
 */

#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief 2-連結認識アルゴリズムの選択
 */
enum class BiconnectedAlgorithm {
    DFS /**< Tarjan の切断点検出 (デフォルト) */
};

/**
 * @brief 2-連結認識の結果
 */
struct BiconnectedResult {
    bool is_biconnected = false; /**< 2-連結グラフであれば true */
};

/**
 * @brief グラフが 2-連結か判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DFS)
 * @return BiconnectedResult
 *
 * 2-連結グラフ: 頂点数 >= 3、連結、切断点なし。
 * Tarjan の DFS で O(n+m) 時間。
 */
inline BiconnectedResult check_biconnected(const Graph& g,
    BiconnectedAlgorithm algo = BiconnectedAlgorithm::DFS) {
    (void)algo;
    BiconnectedResult res;
    res.is_biconnected = false;

    int n = g.n;
    if (n < 3) return res;

    // Tarjan's algorithm for articulation point detection (iterative DFS)
    std::vector<int> tin(n + 1, 0);
    std::vector<int> low(n + 1, 0);
    int timer = 0;

    bool has_artic = false;

    // DFS stack frame
    struct Frame {
        int v, parent;
        size_t i;       // index into adj[v]
        int children;   // DFS children count (for root check)
    };
    std::vector<Frame> stack;

    // Start DFS from vertex 1
    timer++;
    tin[1] = low[1] = timer;
    Frame f0;
    f0.v = 1;
    f0.parent = -1;
    f0.i = 0;
    f0.children = 0;
    stack.push_back(f0);

    while (!stack.empty() && !has_artic) {
        Frame& f = stack.back();
        int v = f.v;

        if (f.i < g.adj[v].size()) {
            int u = g.adj[v][f.i];
            f.i++;

            if (u == f.parent) continue;

            if (tin[u] == 0) {
                // Tree edge
                f.children++;
                timer++;
                tin[u] = low[u] = timer;
                Frame fn;
                fn.v = u;
                fn.parent = v;
                fn.i = 0;
                fn.children = 0;
                stack.push_back(fn);
            } else {
                // Back edge
                low[v] = std::min(low[v], tin[u]);
            }
        } else {
            // Finished processing vertex v
            int v_children = f.children;
            stack.pop_back();
            if (!stack.empty()) {
                Frame& pf = stack.back();
                int pv = pf.v;
                low[pv] = std::min(low[pv], low[v]);

                // Articulation point check (non-root)
                if (pf.parent != -1 && low[v] >= tin[pv]) {
                    has_artic = true;
                }
            } else {
                // Root: articulation if >= 2 DFS children
                if (v_children >= 2) {
                    has_artic = true;
                }
            }
        }
    }

    if (has_artic) return res;

    // Check connectivity: all vertices must be visited
    int visited = 0;
    for (int v = 1; v <= n; ++v) {
        if (tin[v] != 0) visited++;
    }
    if (visited != n) return res;

    res.is_biconnected = true;
    return res;
}

} // namespace graph_recognition

#endif
