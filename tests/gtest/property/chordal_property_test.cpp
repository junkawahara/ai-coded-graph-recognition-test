#include "recognizers/chordal.h"
#include "bf_oracles.h"
#include "certificates.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ChordalAlgorithm;
using graph_recognition::ChordalResult;
using graph_recognition::ObstructionKind;
using graph_recognition::check_chordal;
using graph_recognition::gtest_utils::verify_obstruction;

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
        const ChordalAlgorithm algos[] = {ChordalAlgorithm::MCS_PEO,
                                          ChordalAlgorithm::BUCKET_MCS_PEO,
                                          ChordalAlgorithm::LEXBFS_PEO};
        const char* names[] = {"MCS_PEO", "BUCKET_MCS_PEO", "LEXBFS_PEO"};
        bool bf = bf_is_chordal(n, edges);

        for (size_t a = 0; a < 3; ++a) {
            ChordalResult r = check_chordal(g, algos[a]);
            ASSERT_EQ(r.is_chordal, bf) << names[a] << " trial=" << trial << " n=" << n;
            if (r.is_chordal) continue;
            ASSERT_EQ(r.obstruction.kind, ObstructionKind::HOLE)
                << names[a] << " trial=" << trial << " n=" << n;
            ASSERT_TRUE(verify_obstruction(g, r.obstruction))
                << names[a] << " trial=" << trial << " n=" << n;
        }
    }
}

}  // namespace
