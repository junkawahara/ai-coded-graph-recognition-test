#ifndef GRAPH_RECOGNITION_THREE_LEAF_POWER_H
#define GRAPH_RECOGNITION_THREE_LEAF_POWER_H

/**
 * @file three_leaf_power.h
 * @brief 3-leaf power グラフ認識
 *
 * グラフ G が 3-leaf power であるとは、木 T が存在し T の葉が G の頂点で、
 * 葉 u, v が G で隣接 ⟺ T 上の距離 d(u,v) <= 3 であるもの。
 *
 * 特徴づけ (Brandstädt & Le, 2006):
 *   G が 3-leaf power ⟺ G は (bull, dart, gem)-free な弦グラフ
 *                       ⟺ G の critical clique graph が森
 *
 * アルゴリズム (O(n+m)):
 *   1. 弦グラフか判定
 *   2. critical clique (最大真双子クラス) を計算
 *   3. critical clique graph を構築
 *   4. critical clique graph が森か判定
 *   5. 隣接 critical clique 間の辺が完全二部グラフか検証
 *
 * 参考文献:
 *   - Brandstädt & Le (2006). Structure and linear time recognition
 *     of 3-leaf powers. IPL 98:133-138.
 *   - Dom, Guo, Hüffner, Niedermeier (2006). Error compensation in
 *     leaf power problems. Algorithmica 44(4):363-381.
 *   - Nishimura, Ragde, Thilikos (2002). On graph powers for
 *     leaf-labeled trees. J. Algorithms 42(1):69-108.
 */

#include "chordal.h"
#include "graph.h"
#include <algorithm>
#include <vector>

namespace graph_recognition {

struct ThreeLeafPowerResult {
    bool is_three_leaf_power = false;
};

namespace detail {

/**
 * @brief 3-leaf power 認識: critical clique graph が森かどうかで判定
 *
 * 手順:
 *   1. 弦グラフ判定
 *   2. critical clique 計算 (閉近傍が同じ頂点の最大集合)
 *   3. critical clique graph 構築 + 森判定
 *   4. 隣接 critical clique 間の辺数が完全二部か検証
 */
inline ThreeLeafPowerResult check_three_leaf_power_impl(const Graph& g) {
    ThreeLeafPowerResult res;
    res.is_three_leaf_power = false;

    if (g.n == 0) { res.is_three_leaf_power = true; return res; }

    // 1. 弦グラフ判定
    ChordalResult cr = check_chordal(g);
    if (!cr.is_chordal) return res;

    // 2. Critical clique 計算
    // 閉近傍 N[v] が同一の頂点を同じ critical clique にまとめる。
    // N[v] のソート済みリストをハッシュしてグルーピング。

    // 各頂点の閉近傍ソート済みリストを作成
    std::vector<std::vector<int>> closed_nbr(g.n + 1);
    for (int v = 1; v <= g.n; ++v) {
        closed_nbr[v] = g.adj[v];
        closed_nbr[v].push_back(v);
        std::sort(closed_nbr[v].begin(), closed_nbr[v].end());
    }

    // 閉近傍でグルーピング → critical clique
    // 辞書順でソートしてグルーピング
    std::vector<int> order(g.n);
    for (int i = 0; i < g.n; ++i) order[i] = i + 1;
    std::sort(order.begin(), order.end(),
              [&closed_nbr](int a, int b) {
                  return closed_nbr[a] < closed_nbr[b];
              });

    // cc_id[v] = v が属する critical clique の ID (0-indexed)
    std::vector<int> cc_id(g.n + 1, -1);
    int num_cc = 0;
    std::vector<std::vector<int>> cc_members; // cc_members[i] = クリーク i のメンバー

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

    // 3. Critical clique graph の構築
    // cc_adj[i] = クリーク i に隣接するクリークの集合
    // 辺は G の辺に基づく: u, v が隣接かつ cc_id[u] != cc_id[v]
    std::vector<std::vector<int>> cc_adj(num_cc);
    // 重複を避けるため、各 cc ペアを1回だけ記録
    std::vector<int> seen(num_cc, -1); // seen[j] = i ならクリーク j はクリーク i から既に追加済み

    for (int ci = 0; ci < num_cc; ++ci) {
        int rep = cc_members[ci][0]; // 代表頂点
        for (size_t k = 0; k < g.adj[rep].size(); ++k) {
            int w = g.adj[rep][k];
            int cj = cc_id[w];
            if (cj > ci && seen[cj] != ci) {
                seen[cj] = ci;
                cc_adj[ci].push_back(cj);
                cc_adj[cj].push_back(ci);
            }
        }
    }

    // 4. Critical clique graph が森 (木の集合) か判定
    // 森 ⟺ |E| == |V| - (連結成分数)
    int edge_count = 0;
    for (int ci = 0; ci < num_cc; ++ci) {
        edge_count += (int)cc_adj[ci].size();
    }
    edge_count /= 2;

    // 連結成分数を BFS で計算
    std::vector<int> visited(num_cc, 0);
    int components = 0;
    for (int start = 0; start < num_cc; ++start) {
        if (visited[start]) continue;
        ++components;
        std::vector<int> queue;
        queue.push_back(start);
        visited[start] = 1;
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (size_t k = 0; k < cc_adj[u].size(); ++k) {
                int v = cc_adj[u][k];
                if (!visited[v]) {
                    visited[v] = 1;
                    queue.push_back(v);
                }
            }
        }
    }

    if (edge_count != num_cc - components) return res;

    // 5. 隣接 critical clique 間の辺が完全二部グラフか検証
    // クリーク ci (サイズ a) とクリーク cj (サイズ b) が cc_adj で隣接なら、
    // G 上で ci の全頂点と cj の全頂点が隣接している必要がある。
    //
    // 代表頂点 rep の次数 - (自クリークサイズ - 1) = 隣接する他クリークの頂点数
    // 一方、cc_adj で隣接するクリークのサイズの合計がこれと一致すべき。

    for (int ci = 0; ci < num_cc; ++ci) {
        int rep = cc_members[ci][0];
        int my_size = (int)cc_members[ci].size();
        int external_edges = (int)g.adj[rep].size() - (my_size - 1);

        int expected = 0;
        for (size_t k = 0; k < cc_adj[ci].size(); ++k) {
            expected += (int)cc_members[cc_adj[ci][k]].size();
        }

        if (external_edges != expected) return res;
    }

    res.is_three_leaf_power = true;
    return res;
}

} // namespace detail

/**
 * @brief グラフが 3-leaf power か判定する
 * @param g 入力グラフ
 * @return ThreeLeafPowerResult
 */
inline ThreeLeafPowerResult check_three_leaf_power(const Graph& g) {
    return detail::check_three_leaf_power_impl(g);
}

} // namespace graph_recognition

#endif
