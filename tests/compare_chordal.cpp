#include "chordal.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

/**
 * @brief brute-force: 長さ 4 以上の誘導閉路が存在しないかチェック
 * 弦グラフ ⟺ 長さ >= 4 の誘導閉路なし
 * n <= 9 程度で全部分集合列挙
 */
static bool bf_is_chordal(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }

    // 4 頂点の誘導 C4 チェック
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    int v[4] = {a, b, c, d};
                    // 全 24 順列で C4 チェック
                    for (int p0 = 0; p0 < 4; ++p0) {
                        for (int p1 = 0; p1 < 4; ++p1) {
                            if (p1 == p0) continue;
                            for (int p2 = 0; p2 < 4; ++p2) {
                                if (p2 == p0 || p2 == p1) continue;
                                int p3 = 6 - p0 - p1 - p2;
                                int w0 = v[p0], w1 = v[p1], w2 = v[p2], w3 = v[p3];
                                // C4: w0-w1-w2-w3-w0
                                if (adj[w0][w1] && adj[w1][w2] && adj[w2][w3] && adj[w3][w0] &&
                                    !adj[w0][w2] && !adj[w1][w3]) {
                                    return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // 5..n 頂点の誘導閉路チェック (ビットマスク列挙)
    for (int k = 5; k <= n; ++k) {
        // k 頂点の全部分集合
        int full = (1 << n);
        for (int mask = 0; mask < full; ++mask) {
            if (__builtin_popcount(mask) != k) continue;
            // 部分集合の頂点を取り出す
            std::vector<int> vs;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) vs.push_back(i + 1);
            }
            // 全頂点の次数 (誘導部分グラフ上) を計算
            std::vector<int> deg(k, 0);
            for (int i = 0; i < k; ++i) {
                for (int j = i + 1; j < k; ++j) {
                    if (adj[vs[i]][vs[j]]) {
                        deg[i]++;
                        deg[j]++;
                    }
                }
            }
            // 全頂点の次数がちょうど 2 → 誘導閉路
            bool all_deg2 = true;
            for (int i = 0; i < k; ++i) {
                if (deg[i] != 2) { all_deg2 = false; break; }
            }
            if (!all_deg2) continue;
            // 連結チェック (BFS)
            std::vector<bool> visited(k, false);
            std::vector<int> queue;
            queue.push_back(0);
            visited[0] = true;
            size_t qi = 0;
            while (qi < queue.size()) {
                int ci = queue[qi++];
                for (int j = 0; j < k; ++j) {
                    if (!visited[j] && adj[vs[ci]][vs[j]]) {
                        visited[j] = true;
                        queue.push_back(j);
                    }
                }
            }
            bool connected = true;
            for (int i = 0; i < k; ++i) {
                if (!visited[i]) { connected = false; break; }
            }
            if (connected) return false; // 誘導閉路発見
        }
    }
    return true;
}

/** @brief ランダム弦グラフを生成 (simplicial elimination で構築) */
static std::vector<std::pair<int, int>> gen_random_chordal(int n) {
    std::vector<std::pair<int, int>> edges;
    // 頂点を 1 つずつ追加。各頂点は既存のクリークの部分集合に隣接
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (int v = 2; v <= n; ++v) {
        // v の隣接先を決める: 既存頂点のうちランダムに選び、それが含むクリークに接続
        int num_nbrs = rand() % v; // 0 ~ v-1 個
        if (num_nbrs == 0) num_nbrs = 1;
        if (num_nbrs >= v) num_nbrs = v - 1;
        // ランダムに 1 つの頂点を選び、その近傍 + 自身からサブセットを選ぶ
        int seed = 1 + rand() % (v - 1);
        std::vector<int> candidates;
        candidates.push_back(seed);
        for (int u = 1; u < v; ++u) {
            if (u != seed && adj[seed][u]) {
                candidates.push_back(u);
            }
        }
        // candidates はクリーク。ここからランダムに num_nbrs 個選ぶ
        int cnt = 0;
        for (size_t i = 0; i < candidates.size() && cnt < num_nbrs; ++i) {
            if (rand() % ((int)candidates.size() - (int)i) < num_nbrs - cnt) {
                int u = candidates[i];
                edges.push_back(std::make_pair(u, v));
                adj[u][v] = adj[v][u] = true;
                cnt++;
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
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            n = 1 + rand() % 9;
            edges = gen_random_chordal(n);
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
        ChordalResult r1 = check_chordal(g, ChordalAlgorithm::MCS_PEO);
        ChordalResult r2 = check_chordal(g, ChordalAlgorithm::BUCKET_MCS_PEO);
        bool bf = bf_is_chordal(n, edges);

        if (r1.is_chordal != r2.is_chordal) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " PQ=" << r1.is_chordal
                      << " BUCKET=" << r2.is_chordal << std::endl;
            mismatch++;
        }
        if (r1.is_chordal != bf) {
            std::cerr << "PQ wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " PQ=" << r1.is_chordal
                      << " bf=" << bf << std::endl;
            mismatch++;
        }
        if (r2.is_chordal != bf) {
            std::cerr << "BUCKET wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " BUCKET=" << r2.is_chordal
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
