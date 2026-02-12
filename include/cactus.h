#ifndef GRAPH_RECOGNITION_CACTUS_H
#define GRAPH_RECOGNITION_CACTUS_H

/**
 * @file cactus.h
 * @brief カクタスグラフ (cactus graph) 認識
 *
 * 各二重連結成分が 1 辺または単純サイクルであればカクタス。
 */

#include "graph.h"
#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief カクタスグラフ認識アルゴリズムの選択
 */
enum class CactusAlgorithm {
    DFS /**< DFS による二重連結成分分解 */
};

/**
 * @brief カクタスグラフ認識の結果
 */
struct CactusResult {
    bool is_cactus; /**< カクタスグラフであれば true */
};

namespace detail_cactus {

/** @brief カクタス判定用 DFS チェッカー (内部クラス) */
class CactusChecker {
public:
    explicit CactusChecker(const Graph& graph)
        : g(graph),
          adj(g.n + 1),
          tin(g.n + 1, 0),
          low(g.n + 1, 0),
          mark(g.n + 1, 0),
          comp_deg(g.n + 1, 0),
          timer(0),
          mark_token(0) {
        build_simple_graph();
    }

    bool run() {
        for (int v = 1; v <= g.n; ++v) {
            if (tin[v] != 0) continue;
            if (!dfs(v, -1)) return false;
            if (!edge_stack.empty()) return false;
        }
        return true;
    }

private:
    struct UEdge {
        int u, v;
    };

    const Graph& g;
    std::vector<UEdge> edges;
    std::vector<std::vector<std::pair<int, int>>> adj;
    std::vector<int> tin, low;
    std::vector<int> edge_stack;
    std::vector<int> mark;
    std::vector<int> comp_deg;
    int timer;
    int mark_token;

    void build_simple_graph() {
        for (int u = 1; u <= g.n; ++u) {
            for (std::unordered_set<int>::const_iterator it = g.adj_set[u].begin();
                 it != g.adj_set[u].end(); ++it) {
                int v = *it;
                if (u >= v) continue;
                int eid = (int)edges.size();
                UEdge e;
                e.u = u;
                e.v = v;
                edges.push_back(e);
                adj[u].push_back(std::make_pair(v, eid));
                adj[v].push_back(std::make_pair(u, eid));
            }
        }
    }

    bool pop_component_and_check_cactus(int stop_eid) {
        if (mark_token == INT_MAX) {
            std::fill(mark.begin(), mark.end(), 0);
            mark_token = 0;
        }
        mark_token++;

        std::vector<int> verts;
        int edge_count = 0;

        while (true) {
            if (edge_stack.empty()) return false;
            int eid = edge_stack.back();
            edge_stack.pop_back();
            edge_count++;

            int a = edges[eid].u;
            int b = edges[eid].v;

            if (mark[a] != mark_token) {
                mark[a] = mark_token;
                comp_deg[a] = 0;
                verts.push_back(a);
            }
            if (mark[b] != mark_token) {
                mark[b] = mark_token;
                comp_deg[b] = 0;
                verts.push_back(b);
            }

            comp_deg[a]++;
            comp_deg[b]++;

            if (eid == stop_eid) break;
        }

        if (edge_count == 1) return true;

        if ((int)verts.size() < 3) return false;
        if (edge_count != (int)verts.size()) return false;

        for (size_t i = 0; i < verts.size(); ++i) {
            if (comp_deg[verts[i]] != 2) return false;
        }

        return true;
    }

    bool dfs(int start, int start_parent_eid) {
        struct Frame {
            int v, parent_eid;
            size_t i;
        };
        std::vector<Frame> stack;
        Frame f0;
        f0.v = start;
        f0.parent_eid = start_parent_eid;
        f0.i = 0;
        stack.push_back(f0);
        tin[start] = low[start] = ++timer;

        while (!stack.empty()) {
            Frame& f = stack.back();
            int v = f.v;

            if (f.i < adj[v].size()) {
                int to = adj[v][f.i].first;
                int eid = adj[v][f.i].second;
                f.i++;

                if (eid == f.parent_eid) continue;

                if (tin[to] == 0) {
                    edge_stack.push_back(eid);
                    tin[to] = low[to] = ++timer;
                    Frame fn;
                    fn.v = to;
                    fn.parent_eid = eid;
                    fn.i = 0;
                    stack.push_back(fn);
                } else if (tin[to] < tin[v]) {
                    edge_stack.push_back(eid);
                    low[v] = std::min(low[v], tin[to]);
                }
            } else {
                stack.pop_back();
                if (!stack.empty()) {
                    Frame& parent = stack.back();
                    int pv = parent.v;
                    int eid = adj[pv][parent.i - 1].second;
                    low[pv] = std::min(low[pv], low[v]);
                    if (low[v] >= tin[pv]) {
                        if (!pop_component_and_check_cactus(eid)) return false;
                    }
                }
            }
        }

        return true;
    }
};

} // namespace detail_cactus

/**
 * @brief グラフがカクタスグラフか判定する
 * @param g 入力グラフ
 * @return CactusResult
 */
inline CactusResult check_cactus(const Graph& g,
    CactusAlgorithm algo = CactusAlgorithm::DFS) {
    (void)algo;
    CactusResult res;
    detail_cactus::CactusChecker checker(g);
    res.is_cactus = checker.run();
    return res;
}

} // namespace graph_recognition

#endif
