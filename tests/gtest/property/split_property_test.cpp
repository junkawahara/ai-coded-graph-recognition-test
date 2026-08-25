#include "certificates.h"
#include "split.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::SplitAlgorithm;
using graph_recognition::SplitResult;
using graph_recognition::build_split_obstruction;
using graph_recognition::check_split;
using graph_recognition::gtest_utils::verify_obstruction;

/**
 * @brief {2K2, C4, C5}-free かどうかを brute-force で検査
 *
 * スプリットグラフ ⟺ {2K2, C4, C5}-free なので、独立した正解検証に使う。
 */
bool is_split_brute_force(int n, const std::vector<std::pair<int, int> >& edges) {
    // 隣接行列を構築
    std::vector<std::vector<bool> > adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }

    // 全 4 頂点組を列挙して 2K2 と C4 を探す
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    int v[4];
                    v[0] = a; v[1] = b; v[2] = c; v[3] = d;

                    // C4 チェック: 3 つの非同値な巡回順
                    // (0-1-2-3), (0-1-3-2), (0-2-1-3)
                    int cyc[3][4] = {
                        {0, 1, 2, 3},
                        {0, 1, 3, 2},
                        {0, 2, 1, 3}
                    };
                    for (int p = 0; p < 3; ++p) {
                        int p0 = v[cyc[p][0]], p1 = v[cyc[p][1]];
                        int p2 = v[cyc[p][2]], p3 = v[cyc[p][3]];
                        if (adj[p0][p1] && adj[p1][p2] && adj[p2][p3] && adj[p3][p0]
                            && !adj[p0][p2] && !adj[p1][p3]) {
                            return false;
                        }
                    }

                    // 2K2 チェック: 3 つのマッチング
                    // {a,b},{c,d} / {a,c},{b,d} / {a,d},{b,c}
                    if (adj[a][b] && adj[c][d] && !adj[a][c] && !adj[a][d] && !adj[b][c] && !adj[b][d])
                        return false;
                    if (adj[a][c] && adj[b][d] && !adj[a][b] && !adj[a][d] && !adj[c][b] && !adj[c][d])
                        return false;
                    if (adj[a][d] && adj[b][c] && !adj[a][b] && !adj[a][c] && !adj[d][b] && !adj[d][c])
                        return false;
                }
            }
        }
    }

    // 全 5 頂点組を列挙して C5 を探す
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    for (int e = d + 1; e <= n; ++e) {
                        int v[5];
                        v[0] = a; v[1] = b; v[2] = c; v[3] = d; v[4] = e;

                        // 12 の非同値な巡回順 (fix v[0], permute rest, one direction)
                        // 4!/2 = 12
                        int perms[12][4] = {
                            {1, 2, 3, 4}, {1, 2, 4, 3}, {1, 3, 2, 4}, {1, 3, 4, 2},
                            {1, 4, 2, 3}, {1, 4, 3, 2}, {2, 1, 3, 4}, {2, 1, 4, 3},
                            {2, 3, 1, 4}, {2, 4, 1, 3}, {3, 1, 2, 4}, {3, 2, 1, 4}
                        };
                        for (int p = 0; p < 12; ++p) {
                            int p0 = v[0], p1 = v[perms[p][0]], p2 = v[perms[p][1]];
                            int p3 = v[perms[p][2]], p4 = v[perms[p][3]];
                            // C5: p0-p1-p2-p3-p4-p0
                            bool cycle = adj[p0][p1] && adj[p1][p2] && adj[p2][p3]
                                         && adj[p3][p4] && adj[p4][p0];
                            bool no_chord = !adj[p0][p2] && !adj[p1][p3]
                                            && !adj[p2][p4] && !adj[p3][p0] && !adj[p4][p1];
                            if (cycle && no_chord) return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

/**
 * @brief ランダムスプリットグラフを生成する
 *
 * 最初の k 頂点をクリーク、残りを独立集合とし、
 * 独立集合からクリークへランダムに辺を張る。
 */
std::vector<std::pair<int, int> > gen_random_split_graph(int n) {
    std::vector<std::pair<int, int> > edges;
    if (n <= 1) return edges;

    int k = 1 + std::rand() % n; // クリークサイズ: 1 <= k <= n

    // クリーク部分 (頂点 1..k)
    for (int u = 1; u <= k; ++u) {
        for (int v = u + 1; v <= k; ++v) {
            edges.push_back(std::make_pair(u, v));
        }
    }

    // 独立集合 (頂点 k+1..n) からクリークへランダムに辺
    for (int u = k + 1; u <= n; ++u) {
        for (int v = 1; v <= k; ++v) {
            if (std::rand() % 2 == 1) {
                edges.push_back(std::make_pair(u, v));
            }
        }
    }

    return edges;
}

TEST(SplitProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int> > edges;

        if (trial % 3 == 0) {
            // ランダムスプリットグラフ
            n = 1 + std::rand() % 9;
            edges = gen_random_split_graph(n);
        } else {
            // ランダムグラフ
            n = 1 + std::rand() % 9;
            double p;
            if (trial % 3 == 1) {
                p = (std::rand() % 90 + 10) / 100.0; // 0.1-0.99
            } else {
                p = (std::rand() % 50) / 100.0; // 0.0-0.49
            }
            for (int u = 1; u <= n; ++u) {
                for (int v = u + 1; v <= n; ++v) {
                    if ((std::rand() % 1000) / 1000.0 < p) {
                        edges.push_back(std::make_pair(u, v));
                    }
                }
            }
        }

        Graph g(n, edges);

        // 両アルゴリズムで判定
        SplitResult r1 = check_split(g, SplitAlgorithm::DEGREE_SEQUENCE);
        SplitResult r2 = check_split(g, SplitAlgorithm::HAMMER_SIMEONE);

        // brute-force {2K2, C4, C5}-free 検査
        bool bf_is_split = is_split_brute_force(n, edges);

        ASSERT_EQ(r1.is_split, r2.is_split)
            << "DEGREE_SEQUENCE vs HAMMER_SIMEONE trial=" << trial
            << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_split, bf_is_split)
            << "DEGREE_SEQUENCE trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r2.is_split, bf_is_split)
            << "HAMMER_SIMEONE trial=" << trial << " n=" << n << " m=" << edges.size();

        // 分割自体が証明書になっているので、両アルゴリズムの側集合を検証する
        if (bf_is_split) {
            ASSERT_TRUE(graph_recognition::gtest_utils::verify_split_partition(g, r1.side))
                << "DEGREE_SEQUENCE partition trial=" << trial;
            ASSERT_TRUE(graph_recognition::gtest_utils::verify_split_partition(g, r2.side))
                << "HAMMER_SIMEONE partition trial=" << trial;
        } else {
            ASSERT_TRUE(verify_obstruction(g, r1.obstruction))
                << "DEGREE_SEQUENCE witness trial=" << trial;
            ASSERT_TRUE(verify_obstruction(g, build_split_obstruction(g)))
                << "builder trial=" << trial;
        }
    }
}

}  // namespace
