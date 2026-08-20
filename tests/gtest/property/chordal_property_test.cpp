#include "chordal.h"
#include "bf_oracles.h"
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
    return !graph_recognition::gtest_utils::bf_has_induced_cycle_ge(n, adj, 4);
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
