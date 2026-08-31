#include "recognizers/degenerate.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::DegenerateResult;
using graph_recognition::Graph;
using graph_recognition::check_degenerate;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "degenerate";

// Reads the class's parameterized input format: k on the first line, then
// the standard `n m` + edge lines (load_graph cannot be reused).
Graph load_graph_with_k(const std::string& path, int& k) {
    std::ifstream fin(path.c_str());
    int n = 0, m = 0;
    fin >> k >> n >> m;
    std::vector<std::pair<int, int> > edges;
    for (int e = 0; e < m; ++e) {
        int u, v;
        fin >> u >> v;
        edges.push_back(std::make_pair(u, v));
    }
    return Graph(n, edges);
}

// Exact degeneracy straight from the definition: the maximum over all
// nonempty vertex subsets of the minimum degree of the induced subgraph
// (independent of the recognizer's peeling); n <= 5 only.
int brute_force_degeneracy(int n, const std::vector<int>& adj0) {
    if (n == 0) return -1;
    int best = 0;
    for (int s = 1; s < (1 << n); ++s) {
        int mindeg = n;
        for (int v = 0; v < n; ++v) {
            if (!(s & (1 << v))) continue;
            int d = 0;
            for (int b = adj0[v] & s; b != 0; b &= b - 1) ++d;
            if (d < mindeg) mindeg = d;
        }
        if (mindeg > best) best = mindeg;
    }
    return best;
}

// The (k+1)-core straight from the definition: exhaustively delete
// vertices of degree <= k, returning the survivors as a bitmask.
int brute_force_core(int n, const std::vector<int>& adj0, int k) {
    int alive = (1 << n) - 1;
    bool changed = true;
    while (changed) {
        changed = false;
        for (int v = 0; v < n; ++v) {
            if (!(alive & (1 << v))) continue;
            int d = 0;
            for (int b = adj0[v] & alive; b != 0; b &= b - 1) ++d;
            if (d <= k) {
                alive &= ~(1 << v);
                changed = true;
            }
        }
    }
    return alive;
}

// Replays a claimed removal order and returns the maximum number of
// still-remaining neighbors seen at a removal, or -2 if the order is not
// a permutation of [1, n]. Independent certificate check.
int order_width(const Graph& g, const std::vector<int>& order) {
    const int n = g.n;
    if ((int)order.size() != n + 1) return -2;
    std::vector<bool> used(n + 1, false);
    int width = n == 0 ? -1 : 0;
    for (int i = 1; i <= n; ++i) {
        const int v = order[i];
        if (v < 1 || v > n || used[v]) return -2;
        used[v] = true;
        int d = 0;
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            if (!used[g.adj[v][j]]) ++d;
        }
        if (d > width) width = d;
    }
    return width;
}

class DegenerateTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DegenerateTest, MatchesExpected) {
    const std::string stem = GetParam();
    int k = 0;
    Graph g = load_graph_with_k(
        test_path(std::string(kDir) + "/" + stem + ".in"), k);
    std::string exp =
        read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    DegenerateResult r = check_degenerate(g, k);
    ASSERT_EQ(r.is_degenerate, exp == "YES") << "case=" << stem;
    if (r.is_degenerate) {
        // The certifying removal order must be a permutation whose
        // replayed width matches the reported degeneracy and stays <= k.
        const int w = order_width(g, r.removal_order);
        EXPECT_EQ(w, r.degeneracy) << "case=" << stem;
        EXPECT_LE(w, k) << "case=" << stem;
        EXPECT_TRUE(r.core.empty()) << "case=" << stem;
    } else {
        // The core must be nonempty with every vertex having more than
        // k neighbors inside it.
        EXPECT_GT(r.degeneracy, k) << "case=" << stem;
        EXPECT_TRUE(r.removal_order.empty()) << "case=" << stem;
        ASSERT_FALSE(r.core.empty()) << "case=" << stem;
        std::vector<bool> in_core(g.n + 1, false);
        for (size_t i = 0; i < r.core.size(); ++i) in_core[r.core[i]] = true;
        for (size_t i = 0; i < r.core.size(); ++i) {
            const int v = r.core[i];
            int d = 0;
            for (size_t j = 0; j < g.adj[v].size(); ++j) {
                if (in_core[g.adj[v][j]]) ++d;
            }
            EXPECT_GT(d, k) << "case=" << stem << " v=" << v;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DegenerateTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Differential test against the definitional degeneracy and core: every
// graph on up to 5 vertices, every k from -1 to n-1.
TEST(DegenerateBruteForceTest, MatchesBruteForceDegeneracy) {
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int> > pairs;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                pairs.push_back(std::make_pair(u, v));
            }
        }
        for (int mask = 0; mask < (1 << (int)pairs.size()); ++mask) {
            std::vector<std::pair<int, int> > edges;
            std::vector<int> adj(n, 0);
            for (size_t e = 0; e < pairs.size(); ++e) {
                if (mask & (1 << (int)e)) {
                    edges.push_back(pairs[e]);
                    adj[pairs[e].first - 1] |= 1 << (pairs[e].second - 1);
                    adj[pairs[e].second - 1] |= 1 << (pairs[e].first - 1);
                }
            }
            Graph g(n, edges);
            const int d = brute_force_degeneracy(n, adj);
            for (int k = -1; k < n; ++k) {
                DegenerateResult r = check_degenerate(g, k);
                ASSERT_EQ(r.is_degenerate, k >= 0 && d <= k)
                    << "n=" << n << " mask=" << mask << " k=" << k
                    << " d=" << d;
                ASSERT_EQ(r.degeneracy, d)
                    << "n=" << n << " mask=" << mask << " k=" << k;
                if (r.is_degenerate) {
                    ASSERT_EQ(order_width(g, r.removal_order), d)
                        << "n=" << n << " mask=" << mask << " k=" << k;
                } else {
                    // The reported core must be exactly the (k+1)-core.
                    int core_mask = 0;
                    for (size_t i = 0; i < r.core.size(); ++i) {
                        core_mask |= 1 << (r.core[i] - 1);
                    }
                    ASSERT_EQ(core_mask, brute_force_core(n, adj, k))
                        << "n=" << n << " mask=" << mask << " k=" << k;
                    ASSERT_TRUE(std::adjacent_find(r.core.begin(), r.core.end(),
                                                   std::greater_equal<int>()) ==
                                r.core.end())
                        << "core not sorted: n=" << n << " mask=" << mask;
                }
            }
        }
    }
}

// Complete graphs pin the boundary exactly: degeneracy(K_n) = n-1.
TEST(DegenerateCompleteGraphTest, CompleteGraphBoundary) {
    for (int n = 2; n <= 8; ++n) {
        std::vector<std::pair<int, int> > edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                edges.push_back(std::make_pair(u, v));
            }
        }
        Graph g(n, edges);
        DegenerateResult r = check_degenerate(g, n - 2);
        EXPECT_FALSE(r.is_degenerate) << "n=" << n;
        EXPECT_EQ((int)r.core.size(), n) << "n=" << n;
        r = check_degenerate(g, n - 1);
        EXPECT_TRUE(r.is_degenerate) << "n=" << n;
        EXPECT_EQ(r.degeneracy, n - 1) << "n=" << n;
    }
}

// The empty graph is k-degenerate for every k; nonempty graphs are
// rejected for negative k (their 0-core is everything).
TEST(DegenerateEdgeCaseTest, EmptyGraphAndNegativeK) {
    Graph empty(0, std::vector<std::pair<int, int> >());
    DegenerateResult r = check_degenerate(empty, 0);
    EXPECT_TRUE(r.is_degenerate);
    EXPECT_EQ(r.degeneracy, -1);
    r = check_degenerate(empty, -1);
    EXPECT_TRUE(r.is_degenerate);

    Graph single(1, std::vector<std::pair<int, int> >());
    r = check_degenerate(single, -1);
    EXPECT_FALSE(r.is_degenerate);
    EXPECT_EQ(r.degeneracy, 0);
    ASSERT_EQ((int)r.core.size(), 1);
    EXPECT_EQ(r.core[0], 1);
    EXPECT_TRUE(check_degenerate(single, 0).is_degenerate);
}

}  // namespace
