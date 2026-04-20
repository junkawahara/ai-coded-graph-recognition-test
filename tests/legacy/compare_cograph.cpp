#include "cograph.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

/**
 * @brief P4 (長さ 4 のパス) を誘導部分グラフとして含むか brute-force で検査
 *
 * コグラフ ⟺ P4-free なので、独立した正解検証に使う。
 */
static bool has_induced_p4(int n, const std::vector<std::pair<int, int>>& edges) {
    // 隣接行列を構築
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }
    // 全 4 頂点組を列挙して P4 を探す
    for (int a = 1; a <= n; ++a) {
        for (int b = 1; b <= n; ++b) {
            if (b == a) continue;
            if (!adj[a][b]) continue;
            for (int c = 1; c <= n; ++c) {
                if (c == a || c == b) continue;
                if (!adj[b][c]) continue;
                if (adj[a][c]) continue; // a-c は辺がないこと
                for (int d = 1; d <= n; ++d) {
                    if (d == a || d == b || d == c) continue;
                    if (!adj[c][d]) continue;
                    if (adj[a][d]) continue; // a-d は辺がないこと
                    if (adj[b][d]) continue; // b-d は辺がないこと
                    // a - b - c - d が誘導 P4
                    return true;
                }
            }
        }
    }
    return false;
}

/** @brief ランダムコグラフを cotree 再帰構築で生成する */
static std::vector<std::pair<int, int>> gen_random_cograph(int n) {
    if (n <= 1) return std::vector<std::pair<int, int>>();

    // 頂点を 2 つの部分にランダム分割
    int split = 1 + rand() % (n - 1); // 1 <= split <= n-1
    std::vector<std::pair<int, int>> left_edges = gen_random_cograph(split);
    std::vector<std::pair<int, int>> right_edges = gen_random_cograph(n - split);

    // 右側の頂点番号をシフト
    std::vector<std::pair<int, int>> edges = left_edges;
    for (size_t i = 0; i < right_edges.size(); ++i) {
        edges.push_back(std::make_pair(
            right_edges[i].first + split,
            right_edges[i].second + split));
    }

    // series (1) / parallel (0) をランダム選択
    if (rand() % 2 == 1) {
        // series: 左右の全ペアに辺を追加
        for (int u = 1; u <= split; ++u) {
            for (int v = split + 1; v <= n; ++v) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }
    // parallel: 辺を追加しない

    return edges;
}

int main() {
    srand(42);

    int total = 0, yes_count = 0, no_count = 0;
    int mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // ランダムコグラフ
            n = 1 + rand() % 9;
            edges = gen_random_cograph(n);
        } else {
            // ランダムグラフ
            n = 1 + rand() % 9;
            double p;
            if (trial % 3 == 1) {
                p = (rand() % 90 + 10) / 100.0; // 0.1-0.99
            } else {
                p = (rand() % 50) / 100.0; // 0.0-0.49
            }
            for (int u = 1; u <= n; ++u) {
                for (int v = u + 1; v <= n; ++v) {
                    if ((rand() % 1000) / 1000.0 < p) {
                        edges.push_back(std::make_pair(u, v));
                    }
                }
            }
        }

        Graph g(n, edges);

        // 両アルゴリズムで判定
        CographResult r1 = check_cograph(g, CographAlgorithm::COTREE);
        CographResult r2 = check_cograph(g, CographAlgorithm::PARTITION_REFINEMENT);

        // brute-force P4 検査
        bool bf_is_cograph = !has_induced_p4(n, edges);

        if (r1.is_cograph != r2.is_cograph) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " COTREE=" << r1.is_cograph
                      << " PARTITION=" << r2.is_cograph << std::endl;
            mismatch++;
        }

        if (r1.is_cograph != bf_is_cograph) {
            std::cerr << "COTREE wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " COTREE=" << r1.is_cograph
                      << " brute_force=" << bf_is_cograph << std::endl;
            mismatch++;
        }

        if (r2.is_cograph != bf_is_cograph) {
            std::cerr << "PARTITION wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " PARTITION=" << r2.is_cograph
                      << " brute_force=" << bf_is_cograph << std::endl;
            mismatch++;
        }

        total++;
        if (bf_is_cograph) yes_count++;
        else no_count++;
    }

    if (mismatch > 0) {
        std::cout << "FAIL: " << mismatch << " mismatches" << std::endl;
        return 1;
    }

    std::cout << "OK" << std::endl;
    std::cout << "total=" << total
              << " YES=" << yes_count
              << " NO=" << no_count << std::endl;
    return 0;
}
