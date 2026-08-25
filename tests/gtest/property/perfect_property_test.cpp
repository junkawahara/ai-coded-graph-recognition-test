#include "perfect.h"
#include "bf_oracles.h"
#include "certificates.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::PerfectResult;
using graph_recognition::check_perfect;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::gtest_utils::bf_has_odd_hole;

// ---- Independent brute-force oracle -------------------------------------
// Strong Perfect Graph Theorem (Chudnovsky-Robertson-Seymour-Thomas):
// perfect <=> no odd hole in G and no odd hole in the complement of G.
// The odd-hole check enumerates vertex subsets directly (bf_oracles.h) and
// shares no code with the library's search.

bool bf_is_perfect(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    if (bf_has_odd_hole(n, adj)) return false;
    std::vector<std::vector<bool>> co(n + 1, std::vector<bool>(n + 1, false));
    for (int u = 1; u <= n; ++u)
        for (int v = 1; v <= n; ++v) co[u][v] = (u != v && !adj[u][v]);
    return !bf_has_odd_hole(n, co);
}

TEST(PerfectProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 2500; ++trial) {
        int n = 1 + std::rand() % 9;
        double p;
        if (trial % 3 == 0) {
            p = 0.15 + (std::rand() % 30) / 100.0;  // sparse: holes likely
        } else if (trial % 3 == 1) {
            p = 0.5 + (std::rand() % 40) / 100.0;   // dense: antiholes likely
        } else {
            p = (std::rand() % 100) / 100.0;
        }
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                if ((std::rand() % 1000) / 1000.0 < p)
                    edges.push_back(std::make_pair(u, v));

        Graph g(n, edges);
        PerfectResult r = check_perfect(g);
        bool bf = bf_is_perfect(n, edges);
        ASSERT_EQ(r.is_perfect, bf)
            << "impl vs SPGT oracle trial=" << trial << " n=" << n
            << " m=" << edges.size();
        if (!bf) {
            ASSERT_TRUE(verify_obstruction(g, r.obstruction))
                << "odd hole/antihole trial=" << trial << " n=" << n;
        }
    }
}

}  // namespace
