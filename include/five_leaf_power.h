#ifndef GRAPH_RECOGNITION_FIVE_LEAF_POWER_H
#define GRAPH_RECOGNITION_FIVE_LEAF_POWER_H

/**
 * @file five_leaf_power.h
 * @brief 5-leaf power グラフ認識
 *
 * グラフ G が 5-leaf power であるとは、木 T が存在し T の葉が G の頂点で、
 * 葉 u, v が G で隣接 ⟺ T 上の距離 d(u,v) <= 5 であるもの。
 *
 * アルゴリズム:
 *   1. 強弦グラフか判定 (必要条件)
 *   2. critical clique (閉近傍が同一の頂点の最大集合) を計算
 *   3. 商グラフ Q を構築 (CC 間の辺が all-or-nothing か検証)
 *   4. k ノードの全ラベル付き木について subdivision 可能性をチェック
 *      d'(i,j) = d(i,j) + Σ s_e ≤ 3 (yes ペア), ≥ 4 (no ペア)
 *
 * 一般式: k-leaf power では隣接ペア ≤ k-2, 非隣接ペア ≥ k-1。
 *
 * 参考文献:
 *   - Chang, Ko (2007). Recognition of 5-leaf powers.
 *   - Lafond (2023). General k polynomial time recognition.
 *     ACM Trans. Algorithms.
 */

#include "graph.h"
#include "strongly_chordal.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

struct FiveLeafPowerResult {
    bool is_five_leaf_power = false;
};

namespace detail_five_leaf_power {

/**
 * @brief Prufer 列 (長さ k-2) からラベル付き木の辺リストを生成
 * @param seq Prufer 列 (0-indexed, 各要素 0..k-1)
 * @param k ノード数
 * @param edges 出力辺リスト
 */
inline void prufer_decode(const std::vector<int>& seq, int k,
                          std::vector<std::pair<int, int>>& edges) {
    edges.clear();
    if (k <= 1) return;
    if (k == 2) { edges.push_back(std::make_pair(0, 1)); return; }

    std::vector<int> degree(k, 1);
    for (int i = 0; i < (int)seq.size(); ++i) degree[seq[i]]++;

    for (int i = 0; i < (int)seq.size(); ++i) {
        int x = seq[i];
        for (int leaf = 0; leaf < k; ++leaf) {
            if (degree[leaf] == 1) {
                edges.push_back(std::make_pair(leaf, x));
                degree[leaf]--;
                degree[x]--;
                break;
            }
        }
    }
    int a = -1, b = -1;
    for (int i = 0; i < k; ++i) {
        if (degree[i] == 1) {
            if (a < 0) a = i; else b = i;
        }
    }
    edges.push_back(std::make_pair(a, b));
}

/**
 * @brief 木の全点対距離と各ペアのパス上の辺インデックスを BFS で計算
 */
inline void tree_distances_and_paths(
    const std::vector<std::pair<int, int>>& edges, int k,
    std::vector<std::vector<int>>& dist,
    std::vector<std::vector<std::vector<int>>>& paths) {

    // 隣接リスト (ノード, 辺インデックス)
    std::vector<std::vector<std::pair<int, int>>> adj(k);
    for (int i = 0; i < (int)edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u].push_back(std::make_pair(v, i));
        adj[v].push_back(std::make_pair(u, i));
    }

    dist.assign(k, std::vector<int>(k, 0));
    paths.assign(k, std::vector<std::vector<int>>(k));

    std::vector<int> parent_edge(k);
    std::vector<int> parent_node(k);
    std::vector<char> visited(k);

    for (int s = 0; s < k; ++s) {
        for (int i = 0; i < k; ++i) { visited[i] = 0; parent_edge[i] = -1; }
        visited[s] = 1;
        std::vector<int> queue;
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (size_t ei = 0; ei < adj[u].size(); ++ei) {
                int v = adj[u][ei].first;
                int eidx = adj[u][ei].second;
                if (!visited[v]) {
                    visited[v] = 1;
                    dist[s][v] = dist[s][u] + 1;
                    parent_edge[v] = eidx;
                    parent_node[v] = u;
                    queue.push_back(v);
                }
            }
        }
        // パス復元
        for (int t = 0; t < k; ++t) {
            if (t == s) continue;
            paths[s][t].clear();
            int cur = t;
            while (cur != s) {
                paths[s][t].push_back(parent_edge[cur]);
                cur = parent_node[cur];
            }
        }
    }
}

/**
 * @brief 木 T (k ノード) に辺 subdivision を加えて商グラフ Q を実現可能か判定
 *
 * subdivision 変数 s_e ≥ 0 により新距離 d'(i,j) = d(i,j) + Σ_{e on path} s_e
 * Q[i][j]=true → d'(i,j) ≤ 3, Q[i][j]=false → d'(i,j) ≥ 4
 *
 * (5-leaf power: 隣接 ≤ k-2 = 3, 非隣接 ≥ k-1 = 4)
 */
inline bool check_subdivision_feasibility(
    const std::vector<std::pair<int, int>>& tree_edges, int k,
    const std::vector<std::vector<char>>& Q) {

    if (k <= 1) return true;

    std::vector<std::vector<int>> dist;
    std::vector<std::vector<std::vector<int>>> paths;
    tree_distances_and_paths(tree_edges, k, dist, paths);

    int num_edges = (int)tree_edges.size();

    // "yes" ペアの基本距離チェック
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (Q[i][j] && dist[i][j] > 3) return false;
        }
    }

    // 各辺の上限を計算
    std::vector<int> upper(num_edges, 3);

    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (!Q[i][j]) continue;
            const std::vector<int>& path = paths[i][j];
            int slack = 3 - dist[i][j];
            if (slack == 0) {
                for (size_t p = 0; p < path.size(); ++p)
                    upper[path[p]] = 0;
            } else {
                for (size_t p = 0; p < path.size(); ++p)
                    if (upper[path[p]] > slack) upper[path[p]] = slack;
            }
        }
    }

    // "no" 制約チェック
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            if (Q[i][j]) continue;
            const std::vector<int>& path = paths[i][j];
            int needed = 4 - dist[i][j];
            if (needed <= 0) continue;
            int available = 0;
            for (size_t p = 0; p < path.size(); ++p)
                available += upper[path[p]];
            if (available < needed) return false;
        }
    }

    return true;
}

/**
 * @brief 5-leaf power 認識の実装
 */
inline FiveLeafPowerResult check_five_leaf_power_impl(const Graph& g) {
    FiveLeafPowerResult res;
    res.is_five_leaf_power = false;

    if (g.n == 0) { res.is_five_leaf_power = true; return res; }

    // 1. 強弦グラフ判定 (5-leaf power ⊂ strongly chordal)
    StronglyChordalResult scr = check_strongly_chordal(g);
    if (!scr.is_strongly_chordal) return res;

    // 2. Critical clique 計算
    std::vector<std::vector<int>> closed_nbr(g.n + 1);
    for (int v = 1; v <= g.n; ++v) {
        closed_nbr[v] = g.adj[v];
        closed_nbr[v].push_back(v);
        std::sort(closed_nbr[v].begin(), closed_nbr[v].end());
    }

    std::vector<int> order(g.n);
    for (int i = 0; i < g.n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
              [&closed_nbr](int a, int b) {
                  return closed_nbr[a] < closed_nbr[b];
              });

    std::vector<int> cc_id(g.n + 1, -1);
    int num_cc = 0;
    std::vector<std::vector<int>> cc_members;

    for (int i = 0; i < g.n; ) {
        int j = i;
        while (j < g.n && closed_nbr[order[j]] == closed_nbr[order[i]]) {
            cc_id[order[j]] = num_cc;
            ++j;
        }
        cc_members.push_back(std::vector<int>(order.begin() + i, order.begin() + j));
        num_cc++;
        i = j;
    }

    int k = num_cc;

    // 3. 商グラフ Q を構築
    std::vector<std::vector<char>> Q(k, std::vector<char>(k, 0));
    for (int i = 0; i < k; ++i) Q[i][i] = 1;

    std::vector<int> seen(k, -1);
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        for (size_t ei = 0; ei < g.adj[rep].size(); ++ei) {
            int w = g.adj[rep][ei];
            int cj = cc_id[w];
            if (cj != ci && seen[cj] != ci) {
                seen[cj] = ci;
                Q[ci][cj] = 1;
                Q[cj][ci] = 1;
            }
        }
    }

    // 整合性チェック: Q[ci][cj]=1 なら全ペア隣接
    for (int ci = 0; ci < k; ++ci) {
        int rep = cc_members[ci][0];
        int my_size = (int)cc_members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);
        int expected = 0;
        for (int cj = 0; cj < k; ++cj) {
            if (cj != ci && Q[ci][cj])
                expected += (int)cc_members[cj].size();
        }
        if (external_edges != expected) return res;
    }

    // k=1: 完全グラフ → 常に 5-leaf power
    if (k == 1) { res.is_five_leaf_power = true; return res; }

    // 4. 全ラベル付き木を試行して subdivision 可能性をチェック
    if (k == 2) {
        // k=2: 唯一の木 (辺 0-1)
        std::vector<std::pair<int, int>> te;
        te.push_back(std::make_pair(0, 1));
        if (check_subdivision_feasibility(te, 2, Q)) {
            res.is_five_leaf_power = true;
        }
        return res;
    }

    // k ≥ 3: Prufer 列で全木列挙
    int seq_len = k - 2;
    std::vector<int> seq(seq_len, 0);
    std::vector<std::pair<int, int>> te;

    while (true) {
        prufer_decode(seq, k, te);
        if (check_subdivision_feasibility(te, k, Q)) {
            res.is_five_leaf_power = true;
            return res;
        }

        // 次の Prufer 列
        int carry = seq_len - 1;
        while (carry >= 0) {
            seq[carry]++;
            if (seq[carry] < k) break;
            seq[carry] = 0;
            --carry;
        }
        if (carry < 0) break;
    }

    return res;
}

} // namespace detail_five_leaf_power

/**
 * @brief グラフが 5-leaf power か判定する
 * @param g 入力グラフ
 * @return FiveLeafPowerResult
 */
inline FiveLeafPowerResult check_five_leaf_power(const Graph& g) {
    return detail_five_leaf_power::check_five_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
