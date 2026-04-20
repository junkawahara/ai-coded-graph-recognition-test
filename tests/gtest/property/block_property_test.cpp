#include "block.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::BlockAlgorithm;
using graph_recognition::BlockResult;
using graph_recognition::check_block;

/**
 * @brief brute-force: 全二重連結成分がクリークか DFS でチェック
 * 小さいグラフ (n <= 9) 用の独立した実装
 */
bool bf_is_block(int n, const std::vector<std::pair<int, int>>& edges) {
    // 隣接行列
    std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));
    std::vector<std::vector<int> > nbr(n + 1);
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        if (!adj[u][v]) {
            adj[u][v] = adj[v][u] = true;
            nbr[u].push_back(v);
            nbr[v].push_back(u);
        }
    }

    // ダイヤモンド (K4 minus one edge) を探す
    // ダイヤモンド: 4 頂点 a,b,c,d で辺 ab,ac,ad,bc,bd があり cd がない
    // (つまり a が 3 頂点 b,c,d に隣接、b が c,d に隣接、cd は非隣接)
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (!adj[a][b]) continue;
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    int v[4] = {a, b, c, d};
                    // 全 4! 順列でダイヤモンドチェック
                    for (int p0 = 0; p0 < 4; ++p0) {
                        for (int p1 = 0; p1 < 4; ++p1) {
                            if (p1 == p0) continue;
                            for (int p2 = 0; p2 < 4; ++p2) {
                                if (p2 == p0 || p2 == p1) continue;
                                int p3 = 6 - p0 - p1 - p2;
                                int w0 = v[p0], w1 = v[p1], w2 = v[p2], w3 = v[p3];
                                // ダイヤモンド: w0-w1, w0-w2, w0-w3, w1-w2, w1-w3 あり, w2-w3 なし
                                if (adj[w0][w1] && adj[w0][w2] && adj[w0][w3] &&
                                    adj[w1][w2] && adj[w1][w3] && !adj[w2][w3]) {
                                    // ダイヤモンドあり → C4 も含むので弦でない可能性あるが、
                                    // ダイヤモンドは弦グラフに含まれうる (ただし block ではない)
                                    // 直接ブロックグラフかどうか判定するため全体を見る
                                    goto found_diamond;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    goto no_diamond;

found_diamond:
    ; // fall through

no_diamond:
    ;

    // 弦グラフかつダイヤモンドなしかどうかの代わりに、直接定義を使う:
    // 全二重連結成分がクリークかどうかを BFS で確認
    // 二重連結成分を low-link で求める
    {
        std::vector<int> tin(n + 1, 0), low_val(n + 1, 0);
        int timer = 0;
        std::vector<std::pair<int, int> > edge_stack;
        bool result = true;

        for (int s = 1; s <= n; ++s) {
            if (tin[s] != 0) continue;

            // 反復 DFS
            struct Frame {
                int v, parent;
                size_t i;
            };
            std::vector<Frame> stk;
            Frame f0;
            f0.v = s;
            f0.parent = -1;
            f0.i = 0;
            stk.push_back(f0);
            tin[s] = low_val[s] = ++timer;

            while (!stk.empty()) {
                Frame& f = stk.back();
                int v = f.v;

                if (f.i < nbr[v].size()) {
                    int to = nbr[v][f.i];
                    f.i++;

                    if (to == f.parent) continue;

                    if (tin[to] == 0) {
                        edge_stack.push_back(std::make_pair(v, to));
                        tin[to] = low_val[to] = ++timer;
                        Frame fn;
                        fn.v = to;
                        fn.parent = v;
                        fn.i = 0;
                        stk.push_back(fn);
                    } else if (tin[to] < tin[v]) {
                        edge_stack.push_back(std::make_pair(v, to));
                        if (tin[to] < low_val[v]) low_val[v] = tin[to];
                    }
                } else {
                    stk.pop_back();
                    if (!stk.empty()) {
                        Frame& parent_f = stk.back();
                        int pv = parent_f.v;
                        if (low_val[v] < low_val[pv]) low_val[pv] = low_val[v];
                        if (low_val[v] >= tin[pv]) {
                            // 成分を取り出してクリーク検証
                            std::vector<int> comp_verts;
                            std::vector<bool> in_comp(n + 1, false);
                            int edge_count = 0;
                            while (!edge_stack.empty()) {
                                std::pair<int, int> e = edge_stack.back();
                                edge_stack.pop_back();
                                edge_count++;
                                if (!in_comp[e.first]) {
                                    in_comp[e.first] = true;
                                    comp_verts.push_back(e.first);
                                }
                                if (!in_comp[e.second]) {
                                    in_comp[e.second] = true;
                                    comp_verts.push_back(e.second);
                                }
                                if (e.first == pv && e.second == v) break;
                                if (e.first == v && e.second == pv) break;
                            }
                            long long k = (long long)comp_verts.size();
                            long long need = k * (k - 1) / 2;
                            if ((long long)edge_count != need) {
                                result = false;
                            }
                        }
                    }
                }
            }
        }

        return result;
    }
}

/** @brief ランダムブロックグラフを生成 (クリークの木構造) */
std::vector<std::pair<int, int> > gen_random_block(int n) {
    std::vector<std::pair<int, int> > edges;
    if (n <= 1) return edges;

    std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));

    // クリークを 1 つずつ追加して木構造にする
    int placed = 1; // 頂点 1 は配置済み
    while (placed < n) {
        // 新クリークサイズ: 2 ~ min(4, n - placed + 1)
        int max_clique = n - placed + 1;
        if (max_clique > 4) max_clique = 4;
        int clique_size = 2 + std::rand() % (max_clique - 1);
        if (clique_size > max_clique) clique_size = max_clique;

        // 接続先: 既存の頂点 1 つ
        int attach = 1 + std::rand() % placed;

        // 新クリーク: attach + (clique_size - 1) 個の新頂点
        std::vector<int> clique_verts;
        clique_verts.push_back(attach);
        for (int i = 1; i < clique_size && placed < n; ++i) {
            placed++;
            clique_verts.push_back(placed);
        }

        // クリーク内を完全グラフに
        for (size_t a = 0; a < clique_verts.size(); ++a) {
            for (size_t b = a + 1; b < clique_verts.size(); ++b) {
                int u = clique_verts[a], v = clique_verts[b];
                if (!adj[u][v]) {
                    adj[u][v] = adj[v][u] = true;
                    edges.push_back(std::make_pair(u, v));
                }
            }
        }
    }
    return edges;
}

TEST(BlockProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        if (trial % 3 == 0) {
            // ランダムブロックグラフ
            n = 1 + std::rand() % 9;
            edges = gen_random_block(n);
        } else if (trial % 3 == 1) {
            // ランダムグラフ (中密度)
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // ランダムグラフ (低密度)
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        BlockResult r1 = check_block(g, BlockAlgorithm::DFS);
        BlockResult r2 = check_block(g, BlockAlgorithm::CHORDAL_DIAMOND_FREE);
        bool bf = bf_is_block(n, edges);

        ASSERT_EQ(r1.is_block, r2.is_block)
            << "DFS vs CHORDAL_DIAMOND_FREE trial=" << trial
            << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_block, bf)
            << "DFS trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r2.is_block, bf)
            << "CHORDAL_DIAMOND_FREE trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
