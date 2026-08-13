#include "block.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

/**
 * @brief brute-force: 全二重連結成分がクリークか DFS でチェック
 * 小さいグラフ (n <= 9) 用の独立した実装
 */
static bool bf_is_block(int n, const std::vector<std::pair<int, int>>& edges) {
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

    // 弦グラフかつダイヤモンドなしかどうかの代わりに、直接定義を使う:
    // 全二重連結成分がクリークかどうかを BFS で確認
    // 二重連結成分を low-link で求める
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

/** @brief ランダムブロックグラフを生成 (クリークの木構造) */
static std::vector<std::pair<int, int> > gen_random_block(int n) {
    std::vector<std::pair<int, int> > edges;
    if (n <= 1) return edges;

    std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));

    // クリークを 1 つずつ追加して木構造にする
    int placed = 1; // 頂点 1 は配置済み
    while (placed < n) {
        // 新クリークサイズ: 2 ~ min(4, n - placed + 1)
        int max_clique = n - placed + 1;
        if (max_clique > 4) max_clique = 4;
        int clique_size = 2 + rand() % (max_clique - 1);
        if (clique_size > max_clique) clique_size = max_clique;

        // 接続先: 既存の頂点 1 つ
        int attach = 1 + rand() % placed;

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

int main() {
    srand(42);
    int total = 0, yes_count = 0, no_count = 0, mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        if (trial % 3 == 0) {
            // ランダムブロックグラフ
            n = 1 + rand() % 9;
            edges = gen_random_block(n);
        } else if (trial % 3 == 1) {
            // ランダムグラフ (中密度)
            n = 1 + rand() % 9;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // ランダムグラフ (低密度)
            n = 1 + rand() % 9;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        BlockResult r1 = check_block(g, BlockAlgorithm::DFS);
        BlockResult r2 = check_block(g, BlockAlgorithm::CHORDAL_DIAMOND_FREE);
        bool bf = bf_is_block(n, edges);

        if (r1.is_block != r2.is_block) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " DFS=" << r1.is_block
                      << " CHORDAL_DIAMOND_FREE=" << r2.is_block << std::endl;
            mismatch++;
        }
        if (r1.is_block != bf) {
            std::cerr << "DFS wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " DFS=" << r1.is_block
                      << " bf=" << bf << std::endl;
            mismatch++;
        }
        if (r2.is_block != bf) {
            std::cerr << "CHORDAL_DIAMOND_FREE wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " CHORDAL_DIAMOND_FREE=" << r2.is_block
                      << " bf=" << bf << std::endl;
            mismatch++;
        }

        total++;
        if (bf) yes_count++; else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }
    std::cout << "OK" << std::endl;
    std::cout << "total=" << total << " YES=" << yes_count << " NO=" << no_count << std::endl;
    return 0;
}
