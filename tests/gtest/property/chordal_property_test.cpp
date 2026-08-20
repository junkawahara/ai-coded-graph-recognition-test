#include "chordal.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ChordalAlgorithm;
using graph_recognition::ChordalResult;
using graph_recognition::check_chordal;

// Brute-force: chordal iff no induced cycle of length >= 4.
bool bf_is_chordal(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        int u = edges[i].first, v = edges[i].second;
        adj[u][v] = adj[v][u] = true;
    }
    for (int a = 1; a <= n; ++a) {
        for (int b = a + 1; b <= n; ++b) {
            for (int c = b + 1; c <= n; ++c) {
                for (int d = c + 1; d <= n; ++d) {
                    int v[4] = {a, b, c, d};
                    for (int p0 = 0; p0 < 4; ++p0) {
                        for (int p1 = 0; p1 < 4; ++p1) {
                            if (p1 == p0) continue;
                            for (int p2 = 0; p2 < 4; ++p2) {
                                if (p2 == p0 || p2 == p1) continue;
                                int p3 = 6 - p0 - p1 - p2;
                                int w0 = v[p0], w1 = v[p1], w2 = v[p2], w3 = v[p3];
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
    for (int k = 5; k <= n; ++k) {
        int full = (1 << n);
        for (int mask = 0; mask < full; ++mask) {
            if (__builtin_popcount(mask) != k) continue;
            std::vector<int> vs;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) vs.push_back(i + 1);
            }
            std::vector<int> deg(k, 0);
            for (int i = 0; i < k; ++i) {
                for (int j = i + 1; j < k; ++j) {
                    if (adj[vs[i]][vs[j]]) {
                        deg[i]++;
                        deg[j]++;
                    }
                }
            }
            bool all_deg2 = true;
            for (int i = 0; i < k; ++i) {
                if (deg[i] != 2) { all_deg2 = false; break; }
            }
            if (!all_deg2) continue;
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
            if (connected) return false;
        }
    }
    return true;
}

// Random graph biased toward chordal instances: each new vertex v attaches to
// a random subset of {seed} + N(seed) for a random earlier vertex seed. The
// subset is NOT forced to be a clique, so a noticeable fraction (~8%) of the
// outputs are in fact non-chordal. That is fine here: bf_is_chordal() decides
// the ground truth for every trial, the generator only biases the sampling.
std::vector<std::pair<int, int>> gen_random_chordal(int n) {
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (int v = 2; v <= n; ++v) {
        int num_nbrs = std::rand() % v;
        if (num_nbrs == 0) num_nbrs = 1;
        if (num_nbrs >= v) num_nbrs = v - 1;
        int seed = 1 + std::rand() % (v - 1);
        std::vector<int> candidates;
        candidates.push_back(seed);
        for (int u = 1; u < v; ++u) {
            if (u != seed && adj[seed][u]) candidates.push_back(u);
        }
        int cnt = 0;
        for (size_t i = 0; i < candidates.size() && cnt < num_nbrs; ++i) {
            if (std::rand() % ((int)candidates.size() - (int)i) < num_nbrs - cnt) {
                int u = candidates[i];
                edges.push_back(std::make_pair(u, v));
                adj[u][v] = adj[v][u] = true;
                cnt++;
            }
        }
    }
    return edges;
}

TEST(ChordalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);
    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            n = 1 + std::rand() % 9;
            edges = gen_random_chordal(n);
        } else {
            n = 1 + std::rand() % 9;
            double p = (trial % 3 == 1) ? (std::rand() % 90 + 10) / 100.0
                                        : (std::rand() % 50) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        bool r1 = check_chordal(g, ChordalAlgorithm::MCS_PEO).is_chordal;
        bool r2 = check_chordal(g, ChordalAlgorithm::BUCKET_MCS_PEO).is_chordal;
        bool r3 = check_chordal(g, ChordalAlgorithm::LEXBFS_PEO).is_chordal;
        bool bf = bf_is_chordal(n, edges);

        ASSERT_EQ(r1, bf) << "MCS_PEO trial=" << trial << " n=" << n;
        ASSERT_EQ(r2, bf) << "BUCKET_MCS_PEO trial=" << trial << " n=" << n;
        ASSERT_EQ(r3, bf) << "LEXBFS_PEO trial=" << trial << " n=" << n;
    }
}

}  // namespace
