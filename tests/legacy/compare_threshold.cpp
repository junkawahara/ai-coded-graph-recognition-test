#include "threshold.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

/**
 * @brief brute-force: {P4, C4, 2K2}-free チェック
 * 閾値グラフ ⟺ {P4, C4, 2K2}-free
 */
static bool bf_is_threshold(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }
    // 4頂点の全組合せをチェック
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    int v[4] = {a, b, c, d};
                    // 全24順列をチェック (P4, C4, 2K2)
                    for (int p0 = 0; p0 < 4; ++p0) {
                        for (int p1 = 0; p1 < 4; ++p1) {
                            if (p1 == p0) continue;
                            for (int p2 = 0; p2 < 4; ++p2) {
                                if (p2 == p0 || p2 == p1) continue;
                                int p3 = 6 - p0 - p1 - p2;
                                int w0 = v[p0], w1 = v[p1], w2 = v[p2], w3 = v[p3];
                                // P4: w0-w1-w2-w3 (path)
                                if (adj[w0][w1] && adj[w1][w2] && adj[w2][w3] &&
                                    !adj[w0][w2] && !adj[w0][w3] && !adj[w1][w3]) {
                                    return false;
                                }
                                // C4: w0-w1-w2-w3-w0 (cycle)
                                if (adj[w0][w1] && adj[w1][w2] && adj[w2][w3] && adj[w3][w0] &&
                                    !adj[w0][w2] && !adj[w1][w3]) {
                                    return false;
                                }
                                // 2K2: w0-w1 and w2-w3 (two disjoint edges)
                                if (adj[w0][w1] && adj[w2][w3] &&
                                    !adj[w0][w2] && !adj[w0][w3] && !adj[w1][w2] && !adj[w1][w3]) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

/** @brief ランダム閾値グラフを生成 */
static std::vector<std::pair<int, int>> gen_random_threshold(int n) {
    std::vector<std::pair<int, int>> edges;
    std::vector<int> vertices;
    vertices.push_back(1);
    for (int v = 2; v <= n; ++v) {
        if (rand() % 2 == 0) {
            // 全域頂点: 既存全頂点に辺を張る
            for (size_t i = 0; i < vertices.size(); ++i) {
                edges.push_back(std::make_pair(v, vertices[i]));
            }
        }
        // else: 孤立頂点 (辺なし)
        vertices.push_back(v);
    }
    return edges;
}

int main() {
    srand(42);
    int total = 0, yes_count = 0, no_count = 0, mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            n = 1 + rand() % 9;
            edges = gen_random_threshold(n);
        } else if (trial % 3 == 1) {
            n = 1 + rand() % 9;
            double p = (rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + rand() % 9;
            double p = (rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        ThresholdResult r1 = check_threshold(g, ThresholdAlgorithm::DEGREE_SEQUENCE);
        ThresholdResult r2 = check_threshold(g, ThresholdAlgorithm::DEGREE_SEQUENCE_FAST);
        bool bf = bf_is_threshold(n, edges);

        if (r1.is_threshold != r2.is_threshold) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " OLD=" << r1.is_threshold
                      << " FAST=" << r2.is_threshold << std::endl;
            mismatch++;
        }
        if (r1.is_threshold != bf) {
            std::cerr << "OLD wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " OLD=" << r1.is_threshold
                      << " bf=" << bf << std::endl;
            mismatch++;
        }
        if (r2.is_threshold != bf) {
            std::cerr << "FAST wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " FAST=" << r2.is_threshold
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
