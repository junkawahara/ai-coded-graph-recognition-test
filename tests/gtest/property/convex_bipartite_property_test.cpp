#include "recognizers/convex_bipartite.h"
#include "bf_oracles.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::ConvexBipartiteAlgorithm;
using graph_recognition::ConvexBipartiteResult;
using graph_recognition::check_convex_bipartite;
using graph_recognition::gtest_utils::bf_side_orderable;

// ---- Independent brute-force oracle -------------------------------------
// Convex bipartite <=> there is a proper 2-coloring and an ordering of one
// side making every opposite vertex's neighborhood consecutive. All proper
// 2-colorings (both side assignments arise as complementary colorings) and
// all orderings are enumerated exhaustively (n <= 8 here).

bool bf_is_convex_bipartite(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    for (int mask = 0; mask < (1 << n); ++mask) {
        bool proper = true;
        for (size_t i = 0; i < edges.size() && proper; ++i) {
            int bu = (mask >> (edges[i].first - 1)) & 1;
            int bv = (mask >> (edges[i].second - 1)) & 1;
            if (bu == bv) proper = false;
        }
        if (!proper) continue;
        std::vector<int> x, y;
        for (int v = 1; v <= n; ++v) {
            if ((mask >> (v - 1)) & 1) x.push_back(v); else y.push_back(v);
        }
        if (bf_side_orderable(x, y, adj)) return true;
    }
    return false;
}

TEST(ConvexBipartiteProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 4 == 0) {
            // convex bipartite by construction: interval neighborhoods
            n = 2 + std::rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            int other = n - half;
            // X = {1..half}, Y = {half+1..n}
            for (int x = 1; x <= half; ++x) {
                if (other == 0) break;
                int lo = std::rand() % other;
                int hi = lo + std::rand() % (other - lo);
                for (int j = lo; j <= hi; ++j) {
                    edges.push_back(std::make_pair(x, half + 1 + j));
                }
            }
        } else if (trial % 4 == 1) {
            // random bipartite graph
            n = 2 + std::rand() % 7;
            int half = n / 2;
            if (half < 1) half = 1;
            for (int u = 1; u <= half; ++u)
                for (int v = half + 1; v <= n; ++v)
                    if (std::rand() % 3 == 0)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 4 == 2) {
            // random graph (may not be bipartite)
            n = 1 + std::rand() % 8;
            double p = (std::rand() % 60 + 10) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // tree (always bipartite; small trees are typically convex, but
            // the oracle decides the ground truth either way)
            n = 2 + std::rand() % 7;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
        }

        Graph g(n, edges);
        ConvexBipartiteResult r1 = check_convex_bipartite(g, ConvexBipartiteAlgorithm::C1P);
        ConvexBipartiteResult r2 = check_convex_bipartite(g, ConvexBipartiteAlgorithm::BRUTE_FORCE);
        bool bf = bf_is_convex_bipartite(n, edges);

        ASSERT_EQ(r1.is_convex_bipartite, r2.is_convex_bipartite)
            << "C1P vs BRUTE trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_convex_bipartite, bf)
            << "impl vs oracle trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
