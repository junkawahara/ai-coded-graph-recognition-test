#include "proper_interval.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ProperIntervalAlgorithm;
using graph_recognition::ProperIntervalResult;
using graph_recognition::check_proper_interval;

// ---- Independent brute-force oracle -------------------------------------
// proper interval <=> interval and claw-free (Roberts 1969), and
// interval <=> chordal and AT-free (Lekkerkerker-Boland 1962).
// All three ingredients are checked from first principles below, with no
// code shared with the library (which goes through maximal-clique
// consecutive orderings).

// Some vertex subset of size >= minlen induces a chordless cycle.
bool bf_has_induced_cycle_ge(int n, const std::vector<std::vector<bool>>& adj,
                             int minlen) {
    for (int mask = 0; mask < (1 << n); ++mask) {
        int k = __builtin_popcount(mask);
        if (k < minlen) continue;
        std::vector<int> vs;
        for (int i = 0; i < n; ++i)
            if (mask & (1 << i)) vs.push_back(i + 1);
        bool all_deg2 = true;
        for (int i = 0; i < k && all_deg2; ++i) {
            int d = 0;
            for (int j = 0; j < k; ++j)
                if (j != i && adj[vs[i]][vs[j]]) ++d;
            if (d != 2) all_deg2 = false;
        }
        if (!all_deg2) continue;
        std::vector<bool> vis(k, false);
        std::vector<int> stack;
        stack.push_back(0);
        vis[0] = true;
        int seen = 1;
        while (!stack.empty()) {
            int i = stack.back();
            stack.pop_back();
            for (int j = 0; j < k; ++j) {
                if (!vis[j] && adj[vs[i]][vs[j]]) {
                    vis[j] = true;
                    ++seen;
                    stack.push_back(j);
                }
            }
        }
        if (seen == k) return true;  // connected 2-regular = chordless cycle
    }
    return false;
}

// b is reachable from a in G - N[c].
bool bf_connected_avoiding(int n, const std::vector<std::vector<bool>>& adj,
                           int a, int b, int c) {
    std::vector<bool> banned(n + 1, false);
    banned[c] = true;
    for (int w = 1; w <= n; ++w)
        if (adj[c][w]) banned[w] = true;
    if (banned[a] || banned[b]) return false;
    std::vector<bool> vis(n + 1, false);
    std::vector<int> stack;
    stack.push_back(a);
    vis[a] = true;
    while (!stack.empty()) {
        int x = stack.back();
        stack.pop_back();
        if (x == b) return true;
        for (int w = 1; w <= n; ++w) {
            if (adj[x][w] && !vis[w] && !banned[w]) {
                vis[w] = true;
                stack.push_back(w);
            }
        }
    }
    return false;
}

// Asteroidal triple: three pairwise non-adjacent vertices, any two of which
// are connected by a path avoiding the closed neighborhood of the third.
bool bf_has_asteroidal_triple(int n, const std::vector<std::vector<bool>>& adj) {
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            if (adj[a][b]) continue;
            for (int c = b + 1; c <= n; ++c) {
                if (adj[a][c] || adj[b][c]) continue;
                if (bf_connected_avoiding(n, adj, a, b, c) &&
                    bf_connected_avoiding(n, adj, a, c, b) &&
                    bf_connected_avoiding(n, adj, b, c, a)) {
                    return true;
                }
            }
        }
    }
    return false;
}

// Induced claw: a center adjacent to three pairwise non-adjacent vertices.
bool bf_has_claw(int n, const std::vector<std::vector<bool>>& adj) {
    for (int c = 1; c <= n; ++c) {
        for (int a = 1; a <= n; ++a) {
            if (a == c || !adj[c][a]) continue;
            for (int b = a + 1; b <= n; ++b) {
                if (b == c || !adj[c][b] || adj[a][b]) continue;
                for (int d = b + 1; d <= n; ++d) {
                    if (d == c || !adj[c][d] || adj[a][d] || adj[b][d]) continue;
                    return true;
                }
            }
        }
    }
    return false;
}

bool bf_is_proper_interval(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    if (bf_has_induced_cycle_ge(n, adj, 4)) return false;  // not chordal
    if (bf_has_asteroidal_triple(n, adj)) return false;    // not interval
    if (bf_has_claw(n, adj)) return false;                 // not proper
    return true;
}

TEST(ProperIntervalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // proper interval: path-like structure
            n = 1 + std::rand() % 9;
            for (int v = 1; v < n; ++v) {
                int range = 1 + std::rand() % 3;
                for (int u = v + 1; u <= n && u <= v + range; ++u) {
                    edges.push_back(std::make_pair(v, u));
                }
            }
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
        ProperIntervalResult r1 = check_proper_interval(g, ProperIntervalAlgorithm::TRIPLE_LOOP_CLAW_CHECK);
        ProperIntervalResult r2 = check_proper_interval(g, ProperIntervalAlgorithm::FAST_CLAW_CHECK);
        bool bf = bf_is_proper_interval(n, edges);

        ASSERT_EQ(r1.is_proper_interval, r2.is_proper_interval)
            << "PQ vs FAST trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_proper_interval, bf)
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
