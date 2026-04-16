#ifndef GRAPH_RECOGNITION_SNARK_H
#define GRAPH_RECOGNITION_SNARK_H

/**
 * @file snark.h
 * @brief スナーク (snark) 認識
 *
 * スナークは以下を満たす三正則グラフ:
 *   - 橋なし (2-辺連結)
 *   - 周長 (girth) ≥ 5
 *   - 巡回的 4-辺連結 (cyclically 4-edge-connected)
 *   - 辺彩色数 4 (3-辺彩色不可能)
 */

#include "graph.h"

#include <algorithm>
#include <vector>

namespace graph_recognition {

/**
 * @brief スナーク認識アルゴリズムの選択
 */
enum class SnarkAlgorithm {
    COMBINED /**< 複合チェック */
};

/**
 * @brief スナーク認識の結果
 */
struct SnarkResult {
    bool is_snark = false; /**< スナークであれば true */
};

namespace detail {

/**
 * @brief 橋の検出 (DFS)
 * @return 橋が存在すれば true
 */
inline bool has_bridge(const Graph& g) {
    int n = g.n;
    if (n <= 1) return false;
    std::vector<int> disc(n + 1, -1), low(n + 1, 0);
    int timer = 0;
    bool found = false;

    struct DFSState {
        int v, parent, adj_idx;
    };
    std::vector<DFSState> stack;

    for (int s = 1; s <= n && !found; ++s) {
        if (disc[s] != -1) continue;
        disc[s] = low[s] = timer++;
        stack.push_back({s, 0, 0});

        while (!stack.empty() && !found) {
            DFSState& st = stack.back();
            if (st.adj_idx < (int)g.adj[st.v].size()) {
                int u = g.adj[st.v][st.adj_idx++];
                if (disc[u] == -1) {
                    disc[u] = low[u] = timer++;
                    stack.push_back({u, st.v, 0});
                } else if (u != st.parent) {
                    if (disc[u] < low[st.v]) low[st.v] = disc[u];
                }
            } else {
                int v = st.v;
                int p = st.parent;
                stack.pop_back();
                if (!stack.empty()) {
                    if (low[v] < low[stack.back().v])
                        low[stack.back().v] = low[v];
                    if (low[v] > disc[p]) found = true; /* 橋 */
                }
            }
        }
    }
    return found;
}

/**
 * @brief 周長 (最短閉路の長さ) を計算する
 * @return girth (閉路がなければ n+1)
 */
inline int compute_girth(const Graph& g) {
    int n = g.n;
    int girth = n + 1;

    for (int s = 1; s <= n; ++s) {
        /* BFS で s から最短閉路を探す */
        std::vector<int> dist(n + 1, -1);
        std::vector<int> bfs;
        dist[s] = 0;
        bfs.push_back(s);

        for (size_t qi = 0; qi < bfs.size(); ++qi) {
            int v = bfs[qi];
            if (dist[v] >= girth / 2) break; /* これ以上は改善不可 */
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    bfs.push_back(u);
                } else if (dist[u] >= dist[v]) {
                    /* 閉路発見 */
                    int len = dist[v] + dist[u] + 1;
                    if (len < girth) girth = len;
                }
            }
        }
    }
    return girth;
}

/**
 * @brief 辺を除去したグラフで s-t 間の最大フローを計算 (BFS)
 * @param g 元のグラフ
 * @param skip_u, skip_v 除去する辺の端点
 * @param s ソース
 * @param t シンク
 * @param[out] cut_side S 側の頂点集合 (オプション)
 * @return 最大フロー値
 */
inline int max_flow_unit(const Graph& g, int skip_u, int skip_v,
    int s, int t, std::vector<char>* cut_side) {
    int n = g.n;
    /* 辺リスト構築 (双方向) */
    struct Edge { int to, cap; size_t rev; };
    std::vector<std::vector<Edge>> adj(n + 1);

    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u > v) continue;
            if ((u == skip_u && v == skip_v) || (u == skip_v && v == skip_u))
                continue;
            size_t iu = adj[u].size(), iv = adj[v].size();
            adj[u].push_back({v, 1, iv});
            adj[v].push_back({u, 0, iu});
            iv = adj[v].size();
            iu = adj[u].size();
            adj[v].push_back({u, 1, iu});
            adj[u].push_back({v, 0, iv});
        }
    }

    int flow = 0;
    while (flow < 3) {
        /* BFS で増加パスを探す */
        std::vector<int> prev_v(n + 1, -1);
        std::vector<size_t> prev_e(n + 1, 0);
        std::vector<int> bfs;
        prev_v[s] = s;
        bfs.push_back(s);
        bool found = false;
        for (size_t qi = 0; qi < bfs.size() && !found; ++qi) {
            int v = bfs[qi];
            for (size_t i = 0; i < adj[v].size() && !found; ++i) {
                Edge& e = adj[v][i];
                if (e.cap > 0 && prev_v[e.to] == -1) {
                    prev_v[e.to] = v;
                    prev_e[e.to] = i;
                    if (e.to == t) {
                        found = true;
                    } else {
                        bfs.push_back(e.to);
                    }
                }
            }
        }
        if (!found) break;

        /* パスに沿ってフローを流す */
        for (int v = t; v != s;) {
            int p = prev_v[v];
            size_t ei = prev_e[v];
            adj[p][ei].cap--;
            adj[v][adj[p][ei].rev].cap++;
            v = p;
        }
        ++flow;
    }

    /* カット側を返す */
    if (cut_side) {
        cut_side->assign(n + 1, 0);
        std::vector<int> bfs2;
        std::vector<char>& cs = *cut_side;
        cs[s] = 1;
        bfs2.push_back(s);
        for (size_t qi = 0; qi < bfs2.size(); ++qi) {
            int v = bfs2[qi];
            for (size_t i = 0; i < adj[v].size(); ++i) {
                Edge& e = adj[v][i];
                if (e.cap > 0 && !cs[e.to]) {
                    cs[e.to] = 1;
                    bfs2.push_back(e.to);
                }
            }
        }
    }

    return flow;
}

/**
 * @brief 巡回的 4-辺連結チェック
 *
 * 3-辺連結かつ全ての 3-辺カットが自明 (一頂点のスター) であることを確認。
 */
inline bool is_cyclically_4_edge_connected(const Graph& g) {
    int n = g.n;
    if (n < 4) return false;

    /* 3-辺連結チェック: 各辺を除去し、橋の有無を確認 */
    /* 橋があれば 2-辺カットが存在 → 3-辺連結でない */
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (v <= u) continue;
            /* (u,v) を除去した後に橋があるか */
            /* 簡易チェック: DFS で橋検出 */
            std::vector<int> disc(n + 1, -1), low(n + 1, 0);
            int timer = 0;
            bool bridge_found = false;

            struct DFS { int v, par, idx; };
            std::vector<DFS> stk;
            int start = (u == 1 && v == 2) ? ((n > 2) ? 3 : 1) : 1;
            disc[start] = low[start] = timer++;
            stk.push_back({start, 0, 0});

            while (!stk.empty() && !bridge_found) {
                DFS& st = stk.back();
                if (st.idx < (int)g.adj[st.v].size()) {
                    int w = g.adj[st.v][st.idx++];
                    /* スキップ辺 */
                    if ((st.v == u && w == v) || (st.v == v && w == u)) continue;
                    if (disc[w] == -1) {
                        disc[w] = low[w] = timer++;
                        stk.push_back({w, st.v, 0});
                    } else if (w != st.par) {
                        if (disc[w] < low[st.v]) low[st.v] = disc[w];
                    }
                } else {
                    int cv = st.v, cp = st.par;
                    stk.pop_back();
                    if (!stk.empty()) {
                        if (low[cv] < low[stk.back().v])
                            low[stk.back().v] = low[cv];
                        if (low[cv] > disc[cp]) bridge_found = true;
                    }
                }
            }
            if (bridge_found) return false;
        }
    }

    /* 非自明 3-辺カットのチェック:
       各辺 (u,v) を除去し、u-v 間の最大フローが 2 のとき
       カット側が非自明 (両側 ≥ 3 頂点) かチェック */
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (v <= u) continue;

            std::vector<char> cut_side;
            int flow = max_flow_unit(g, u, v, u, v, &cut_side);

            if (flow == 2) {
                /* 3-辺カット発見。カットの両側のサイズをチェック */
                int s_side = 0;
                for (int w = 1; w <= n; ++w) {
                    if (cut_side[w]) ++s_side;
                }
                int t_side = n - s_side;
                if (s_side >= 3 && t_side >= 3) return false;
            }
        }
    }

    return true;
}

/**
 * @brief 3-辺彩色が可能か (バックトラッキング)
 * @return 3-辺彩色可能であれば true
 */
inline bool is_3_edge_colorable(const Graph& g) {
    int n = g.n;
    /* 辺リスト構築 */
    struct UEdge { int u, v; };
    std::vector<UEdge> edges;
    for (int u = 1; u <= n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) edges.push_back({u, v});
        }
    }
    int m = (int)edges.size();
    if (m == 0) return true;

    /* 各頂点の隣接辺インデックス */
    std::vector<std::vector<int>> inc(n + 1);
    for (int i = 0; i < m; ++i) {
        inc[edges[i].u].push_back(i);
        inc[edges[i].v].push_back(i);
    }

    std::vector<int> color(m, 0); /* 0 = 未彩色, 1/2/3 = 色 */

    /* 各頂点で使用済み色をビットマスクで管理 */
    std::vector<int> used_mask(n + 1, 0);

    /* バックトラッキング */
    struct State {
        std::vector<UEdge>& edges;
        std::vector<int>& color;
        std::vector<int>& used_mask;
        std::vector<std::vector<int>>& inc;
        int m;

        bool solve(int idx) {
            if (idx == m) return true;
            int u = edges[idx].u, v = edges[idx].v;
            int forbidden = used_mask[u] | used_mask[v];
            for (int c = 1; c <= 3; ++c) {
                int bit = 1 << c;
                if (forbidden & bit) continue;
                color[idx] = c;
                used_mask[u] |= bit;
                used_mask[v] |= bit;
                if (solve(idx + 1)) return true;
                used_mask[u] &= ~bit;
                used_mask[v] &= ~bit;
            }
            color[idx] = 0;
            return false;
        }
    };

    State state = {edges, color, used_mask, inc, m};
    return state.solve(0);
}

} // namespace detail

/**
 * @brief グラフがスナークか判定する
 * @param g 入力グラフ
 * @param algo 使用アルゴリズム (デフォルト: COMBINED)
 * @return SnarkResult
 *
 * スナーク ⟺ 三正則 ∧ 橋なし ∧ girth ≥ 5 ∧
 *   巡回的 4-辺連結 ∧ 辺彩色数 4。
 */
inline SnarkResult check_snark(const Graph& g,
    SnarkAlgorithm algo = SnarkAlgorithm::COMBINED) {
    (void)algo;
    SnarkResult res;

    int n = g.n;
    if (n < 10) return res; /* 最小スナーク = Petersen (10 頂点) */

    /* 三正則チェック */
    for (int v = 1; v <= n; ++v) {
        if ((int)g.adj[v].size() != 3) return res;
    }

    /* n は偶数 (三正則 → 3n = 2m → n 偶数) */
    if (n % 2 != 0) return res;

    /* 連結性チェック */
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

    /* 橋なし (2-辺連結) */
    if (detail::has_bridge(g)) return res;

    /* 周長 ≥ 5 */
    int girth = detail::compute_girth(g);
    if (girth < 5) return res;

    /* 巡回的 4-辺連結 */
    if (!detail::is_cyclically_4_edge_connected(g)) return res;

    /* 3-辺彩色不可能 */
    if (detail::is_3_edge_colorable(g)) return res;

    res.is_snark = true;
    return res;
}

} // namespace graph_recognition

#endif
