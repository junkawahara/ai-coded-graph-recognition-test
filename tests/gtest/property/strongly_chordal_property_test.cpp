#include "certificates.h"
#include "recognizers/strongly_chordal.h"
#include "bf_oracles.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::StronglyChordalAlgorithm;
using graph_recognition::StronglyChordalResult;
using graph_recognition::check_strongly_chordal;
using graph_recognition::gtest_utils::bf_has_induced_cycle_ge;

// ---- Independent brute-force oracle -------------------------------------
// Farber's characterization: G is strongly chordal iff G is chordal and
// contains no induced sun (trampoline). A k-sun (k >= 3) is 2k vertices
// u_1..u_k, w_1..w_k with {u_i} independent and u_i adjacent to w_j iff
// j = i or j = i+1 (mod k); edges among the w_j are unconstrained.
// This is a different characterization from the simple-vertex elimination
// used by all three library implementations.

struct SunSearch {
    int n;
    int k;
    const std::vector<std::vector<bool>>& adj;
    std::vector<int> u;
    std::vector<bool> used;

    SunSearch(int n_, int k_, const std::vector<std::vector<bool>>& adj_)
        : n(n_), k(k_), adj(adj_), u(k_, 0), used(n_ + 1, false) {}

    // Assign w_j for j.. : w_j must be adjacent to exactly u_{j-1} and u_j
    // among the chosen u's (indices mod k).
    bool pick_w(int j) {
        if (j == k) return true;
        for (int cand = 1; cand <= n; ++cand) {
            if (used[cand]) continue;
            bool ok = true;
            for (int t = 0; t < k && ok; ++t) {
                bool want = (t == j || (t + 1) % k == j);
                if (adj[u[t]][cand] != want) ok = false;
            }
            if (!ok) continue;
            used[cand] = true;
            if (pick_w(j + 1)) return true;
            used[cand] = false;
        }
        return false;
    }

    // Assign u_i for i.. : the u's must be pairwise non-adjacent.
    bool pick_u(int i) {
        if (i == k) return pick_w(0);
        for (int cand = 1; cand <= n; ++cand) {
            if (used[cand]) continue;
            bool ok = true;
            for (int t = 0; t < i && ok; ++t)
                if (adj[u[t]][cand]) ok = false;
            if (!ok) continue;
            used[cand] = true;
            u[i] = cand;
            if (pick_u(i + 1)) return true;
            used[cand] = false;
        }
        return false;
    }
};

bool bf_has_sun(int n, const std::vector<std::vector<bool>>& adj) {
    for (int k = 3; 2 * k <= n; ++k) {
        SunSearch s(n, k, adj);
        if (s.pick_u(0)) return true;
    }
    return false;
}

bool bf_is_strongly_chordal(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    if (bf_has_induced_cycle_ge(n, adj, 4)) return false;  // not chordal
    return !bf_has_sun(n, adj);
}

TEST(StronglyChordalProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 3000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 3 == 0) {
            // Biased toward chordal instances: v attaches to u plus a random
            // subset of N(u), which is NOT necessarily a clique, so the
            // output is not guaranteed (strongly) chordal. That is fine: the
            // brute-force oracle decides the ground truth for every trial.
            n = 1 + std::rand() % 9;
            std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
                adj[u][v] = adj[v][u] = true;
                for (int w = 1; w < v; ++w) {
                    if (w != u && adj[u][w] && std::rand() % 2 == 0) {
                        edges.push_back(std::make_pair(w, v));
                        adj[w][v] = adj[v][w] = true;
                    }
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
        StronglyChordalResult r1 = check_strongly_chordal(g, StronglyChordalAlgorithm::STRONG_ELIMINATION);
        StronglyChordalResult r2 = check_strongly_chordal(g, StronglyChordalAlgorithm::PEO_MATRIX);
        StronglyChordalResult r3 = check_strongly_chordal(g, StronglyChordalAlgorithm::MCS_SEO);
        bool bf = bf_is_strongly_chordal(n, edges);

        ASSERT_EQ(r1.is_strongly_chordal, r2.is_strongly_chordal)
            << "ELIM vs PEO trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r1.is_strongly_chordal, r3.is_strongly_chordal)
            << "ELIM vs MCS_SEO trial=" << trial << " n=" << n << " m=" << edges.size();
        StronglyChordalResult r4 =
            check_strongly_chordal(g, StronglyChordalAlgorithm::FARBER_SEO);
        ASSERT_EQ(r1.is_strongly_chordal, bf)
            << "impl vs sun-free oracle trial=" << trial << " n=" << n << " m=" << edges.size();
        ASSERT_EQ(r4.is_strongly_chordal, bf)
            << "FARBER_SEO vs sun-free oracle trial=" << trial << " n=" << n;

        // Farber の構成が実際に strong elimination ordering になっているか
        if (bf) {
            ASSERT_TRUE(graph_recognition::gtest_utils::verify_seo(
                g, r4.seo_order, r4.seo_number))
                << "SEO trial=" << trial << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
