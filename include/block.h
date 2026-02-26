#ifndef GRAPH_RECOGNITION_BLOCK_H
#define GRAPH_RECOGNITION_BLOCK_H

/**
 * @file block.h
 * @brief ブロックグラフ (block graph) 認識
 *
 * 各二重連結成分がクリークであればブロックグラフと判定する。
 *
 * アルゴリズム:
 *   - DFS: 二重連結成分分解 O(n+m)
 *   - CHORDAL_DIAMOND_FREE: 弦グラフ + ダイヤモンドフリー判定 O(n+m+mΔ)
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <climits>
#include <utility>
#include <vector>

namespace graph_recognition {

/**
 * @brief ブロックグラフ認識アルゴリズムの選択
 */
enum class BlockAlgorithm {
    DFS,                 /**< DFS による二重連結成分分解 (デフォルト) */
    CHORDAL_DIAMOND_FREE /**< 弦グラフ + ダイヤモンドフリー判定 */
};

/**
 * @brief ブロックグラフ認識の結果
 */
struct BlockResult {
    bool is_block; /**< ブロックグラフであれば true */
};

namespace detail {

/** @brief ブロックグラフの DFS ベースチェッカー (内部クラス) */
class BlockChecker {
public:
    explicit BlockChecker(const Graph& graph)
        : g(graph),
          adj(g.n + 1),
          tin(g.n + 1, 0),
          low(g.n + 1, 0),
          mark(g.n + 1, 0),
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

    bool pop_component_and_check_clique(int stop_eid) {
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
                verts.push_back(a);
            }
            if (mark[b] != mark_token) {
                mark[b] = mark_token;
                verts.push_back(b);
            }
            if (eid == stop_eid) break;
        }

        long long k = (long long)verts.size();
        long long need = k * (k - 1) / 2;
        return (long long)edge_count == need;
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
                        if (!pop_component_and_check_clique(eid)) return false;
                    }
                }
            }
        }

        return true;
    }
};

/**
 * @brief ダイヤモンド (K₄⁻) の誘導部分グラフが存在するか判定する
 *
 * ダイヤモンドは 4 頂点 {u, v, w₁, w₂} からなり、w₁w₂ 以外の 5 辺が存在する。
 * 各辺 (u, v) について共通隣接頂点の中に非隣接ペアがあればダイヤモンド。
 *
 * @param g 入力グラフ
 * @return true ダイヤモンドが存在する
 */
inline bool has_diamond(const Graph& g) {
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u >= v) continue;

            // u, v の共通隣接頂点を収集
            std::vector<int> common;
            for (size_t j = 0; j < g.adj[u].size(); ++j) {
                int w = g.adj[u][j];
                if (w != v && g.has_edge(v, w)) {
                    common.push_back(w);
                }
            }

            // 共通隣接頂点の中に非隣接ペアがあればダイヤモンド
            for (size_t a = 0; a < common.size(); ++a) {
                for (size_t b = a + 1; b < common.size(); ++b) {
                    if (!g.has_edge(common[a], common[b])) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

/**
 * @brief 弦グラフ + ダイヤモンドフリーによるブロックグラフ認識
 *
 * ブロックグラフ ⟺ 弦グラフかつダイヤモンド (K₄⁻) を含まない。
 * Bandelt-Mulder (1986) の特性化に基づく。
 *
 * @param g 入力グラフ
 * @return BlockResult
 */
inline BlockResult check_block_chordal_diamond_free(const Graph& g) {
    BlockResult res;
    ChordalResult chordal = check_chordal(g);
    if (!chordal.is_chordal) {
        res.is_block = false;
        return res;
    }
    res.is_block = !has_diamond(g);
    return res;
}

} // namespace detail

/**
 * @brief グラフがブロックグラフか判定する
 * @param g 入力グラフ
 * @param algo 使用するアルゴリズム (デフォルト: DFS)
 * @return BlockResult
 *
 * すべての二重連結成分がクリークであればブロックグラフ。
 */
inline BlockResult check_block(const Graph& g,
    BlockAlgorithm algo = BlockAlgorithm::DFS) {
    switch (algo) {
        case BlockAlgorithm::DFS: {
            BlockResult res;
            detail::BlockChecker checker(g);
            res.is_block = checker.run();
            return res;
        }
        case BlockAlgorithm::CHORDAL_DIAMOND_FREE:
            return detail::check_block_chordal_diamond_free(g);
    }
    return BlockResult();
}

} // namespace graph_recognition

#endif
