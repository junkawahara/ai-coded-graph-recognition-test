#include "cochain.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::CochainAlgorithm;
using graph_recognition::CochainResult;
using graph_recognition::check_cochain;

// ---- Independent brute-force oracle -------------------------------------
// Co-chain <=> the complement is a chain graph <=> the complement is
// bipartite and 2K2-free. Checked from first principles on the complement,
// sharing no code with either library implementation.

bool bf_is_bipartite(int n, const std::vector<std::vector<bool>>& adj) {
    std::vector<int> color(n + 1, -1);
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

bool bf_has_2k2(int n, const std::vector<std::vector<bool>>& adj) {
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (!adj[a][b]) continue;
            for (int c = a + 1; c <= n; ++c) {
                if (c == b) continue;
                for (int d = c + 1; d <= n; ++d) {
                    if (d == a || d == b) continue;
                    if (!adj[c][d]) continue;
                    if (!adj[a][c] && !adj[a][d] && !adj[b][c] && !adj[b][d]) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool bf_is_cochain(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    std::vector<std::vector<bool>> co(n + 1, std::vector<bool>(n + 1, false));
    for (int u = 1; u <= n; ++u)
        for (int v = 1; v <= n; ++v)
            co[u][v] = (u != v && !adj[u][v]);
    return bf_is_bipartite(n, co) && !bf_has_2k2(n, co);
}

TEST(CochainProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // co-bipartite + chain-like: 2 クリーク間の辺を nested にする
            n = 2 + std::rand() % 8;
            int half = n / 2;
            // 2 クリーク
            for (int u = 1; u <= half; ++u)
                for (int v = u + 1; v <= half; ++v)
                    edges.push_back(std::make_pair(u, v));
            for (int u = half + 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    edges.push_back(std::make_pair(u, v));
            // クリーク間: nested 構造
            for (int u = 1; u <= half; ++u)
                for (int v = half + 1; v <= half + u && v <= n; ++v)
                    edges.push_back(std::make_pair(u, v));
        } else if (trial % 3 == 1) {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 90 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        CochainResult r1 = check_cochain(g, CochainAlgorithm::COMPLEMENT);
        CochainResult r2 = check_cochain(g, CochainAlgorithm::DIRECT);
        bool bf = bf_is_cochain(n, edges);

        ASSERT_EQ(r1.is_cochain, r2.is_cochain)
            << "COMPLEMENT vs DIRECT trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_cochain, bf)
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
