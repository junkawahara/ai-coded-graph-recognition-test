#include "chain.h"
#include "graph.h"
#include <cstdlib>
#include <iostream>
#include <utility>
#include <vector>

using namespace graph_recognition;

/**
 * @brief 二部グラフ判定 (BFS, brute-force 用)
 *
 * 二部グラフなら color に 2 彩色を格納して true を返す。
 */
static bool is_bipartite_bf(int n, const std::vector<std::vector<bool> >& adj,
                            std::vector<int>& color) {
    color.assign(n + 1, -1);
    for (int s = 1; s <= n; ++s) {
        if (color[s] != -1) continue;
        color[s] = 0;
        std::vector<int> queue;
        queue.push_back(s);
        for (size_t qi = 0; qi < queue.size(); ++qi) {
            int u = queue[qi];
            for (int v = 1; v <= n; ++v) {
                if (!adj[u][v]) continue;
                if (color[v] == -1) {
                    color[v] = 1 - color[u];
                    queue.push_back(v);
                } else if (color[v] == color[u]) {
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief チェーングラフかどうかを brute-force で検査
 *
 * チェーングラフ ⟺ 二部かつ 2K2-free
 */
static bool is_chain_brute_force(int n, const std::vector<std::pair<int, int> >& edges) {
    // 隣接行列を構築
    std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }

    // 二部判定
    std::vector<int> color;
    if (!is_bipartite_bf(n, adj, color)) return false;

    // 2K2 チェック: 4 頂点から独立な 2 辺を探す
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (!adj[a][b]) continue;
            for (int c = a + 1; c <= n; ++c) {
                if (c == b) continue;
                for (int d = c + 1; d <= n; ++d) {
                    if (d == a || d == b) continue;
                    if (!adj[c][d]) continue;
                    // {a,b} と {c,d} が独立な辺か
                    if (!adj[a][c] && !adj[a][d] && !adj[b][c] && !adj[b][d]) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @brief ランダムチェーングラフを生成する
 *
 * 二部グラフで近傍がネスト: L 側頂点を次数昇順にし、
 * R 側各頂点の近傍が L 側の接尾辞 (prefix) になるよう構成する。
 */
static std::vector<std::pair<int, int> > gen_random_chain_graph(int n) {
    std::vector<std::pair<int, int> > edges;
    if (n <= 1) return edges;

    int left_size = 1 + rand() % n; // 1 <= left_size <= n
    int right_size = n - left_size;
    if (right_size <= 0) return edges;

    // L 側: 頂点 1..left_size, R 側: 頂点 left_size+1..n
    // R 側各頂点の近傍は L の接尾辞 (つまり L 側を次数昇順で並べたとき、
    // 各 R 頂点は L[threshold..left_size] に隣接)
    // threshold をランダムに選ぶ (0 = 全隣接, left_size = 隣接なし)
    for (int r = left_size + 1; r <= n; ++r) {
        int threshold = rand() % (left_size + 1); // 0..left_size
        for (int l = threshold + 1; l <= left_size; ++l) {
            edges.push_back(std::make_pair(r, l));
        }
    }

    return edges;
}

int main() {
    srand(42);

    int total = 0, yes_count = 0, no_count = 0;
    int mismatch = 0;

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        if (trial % 3 == 0) {
            // ランダムチェーングラフ
            n = 1 + rand() % 9;
            edges = gen_random_chain_graph(n);
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
        ChainResult r1 = check_chain(g, ChainAlgorithm::NEIGHBORHOOD_INCLUSION);
        ChainResult r2 = check_chain(g, ChainAlgorithm::DEGREE_SORT);

        // brute-force: 二部 + 2K2-free
        bool bf_is_chain = is_chain_brute_force(n, edges);

        if (r1.is_chain != r2.is_chain) {
            std::cerr << "MISMATCH at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " NEIGHBORHOOD_INCLUSION=" << r1.is_chain
                      << " DEGREE_SORT=" << r2.is_chain << std::endl;
            mismatch++;
        }

        if (r1.is_chain != bf_is_chain) {
            std::cerr << "NEIGHBORHOOD_INCLUSION wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " NEIGHBORHOOD_INCLUSION=" << r1.is_chain
                      << " brute_force=" << bf_is_chain << std::endl;
            mismatch++;
        }

        if (r2.is_chain != bf_is_chain) {
            std::cerr << "DEGREE_SORT wrong at trial " << trial
                      << " n=" << n << " m=" << edges.size()
                      << " DEGREE_SORT=" << r2.is_chain
                      << " brute_force=" << bf_is_chain << std::endl;
            mismatch++;
        }

        total++;
        if (bf_is_chain) yes_count++;
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
