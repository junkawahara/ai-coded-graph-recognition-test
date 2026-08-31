#include "recognizers/partial_ktree.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::PartialKTreeResult;
using graph_recognition::check_partial_ktree;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "partial_ktree";

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

// Exact treewidth by minimizing the elimination width over all n!
// orders (independent of the recognizer's search); n <= 8 only.
int brute_force_treewidth(int n, const std::vector<int>& adj0) {
    if (n == 0) return -1;
    std::vector<int> perm(n);
    for (int i = 0; i < n; ++i) perm[i] = i;
    int best = n - 1;
    do {
        std::vector<int> adj = adj0;
        int width = 0;
        int alive = (1 << n) - 1;
        for (int i = 0; i < n && width < best; ++i) {
            const int v = perm[i];
            alive &= ~(1 << v);
            const int nb = adj[v] & alive;
            int d = 0;
            for (int b = nb; b != 0; b &= b - 1) ++d;
            if (d > width) width = d;
            for (int u = 0; u < n; ++u) {
                if (nb & (1 << u)) adj[u] |= nb & ~(1 << u);
            }
        }
        if (width < best) best = width;
    } while (std::next_permutation(perm.begin(), perm.end()));
    return best;
}

// Replays a claimed elimination order and returns its width, or -2 if the
// order is not a permutation of [1, n]. Independent certificate check.
int order_width(const Graph& g, const std::vector<int>& order) {
    const int n = g.n;
    if ((int)order.size() != n + 1) return -2;
    std::vector<int> adj(n, 0);
    for (int v = 1; v <= n; ++v) {
        for (size_t j = 0; j < g.adj[v].size(); ++j) {
            adj[v - 1] |= 1 << (g.adj[v][j] - 1);
        }
    }
    std::vector<bool> used(n + 1, false);
    int alive = (1 << n) - 1;
    int width = n == 0 ? -1 : 0;
    for (int i = 1; i <= n; ++i) {
        const int v = order[i];
        if (v < 1 || v > n || used[v]) return -2;
        used[v] = true;
        alive &= ~(1 << (v - 1));
        const int nb = adj[v - 1] & alive;
        int d = 0;
        for (int b = nb; b != 0; b &= b - 1) ++d;
        if (d > width) width = d;
        for (int u = 0; u < n; ++u) {
            if (nb & (1 << u)) adj[u] |= nb & ~(1 << u);
        }
    }
    return width;
}

class PartialKTreeTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PartialKTreeTest, MatchesExpected) {
    const std::string stem = GetParam();
    int k = 0;
    Graph g = load_graph_with_k(
        test_path(std::string(kDir) + "/" + stem + ".in"), k);
    std::string exp =
        read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    PartialKTreeResult r = check_partial_ktree(g, k);
    ASSERT_EQ(r.is_partial_ktree, exp == "YES") << "case=" << stem;
    if (r.is_partial_ktree) {
        // The certifying elimination order must be a permutation whose
        // replayed width matches the reported width and stays <= k.
        const int w = order_width(g, r.elimination_order);
        EXPECT_EQ(w, r.width) << "case=" << stem;
        EXPECT_LE(w, k) << "case=" << stem;
    } else {
        EXPECT_EQ(r.width, -1) << "case=" << stem;
        EXPECT_TRUE(r.elimination_order.empty()) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PartialKTreeTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Differential test against brute-force treewidth: every graph on up to
// 5 vertices, every k from 0 to n-1.
TEST(PartialKTreeBruteForceTest, MatchesBruteForceTreewidth) {
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
            const int tw = brute_force_treewidth(n, adj);
            for (int k = 0; k < n; ++k) {
                PartialKTreeResult r = check_partial_ktree(g, k);
                ASSERT_EQ(r.is_partial_ktree, tw <= k)
                    << "n=" << n << " mask=" << mask << " k=" << k
                    << " tw=" << tw;
                if (r.is_partial_ktree) {
                    const int w = order_width(g, r.elimination_order);
                    ASSERT_EQ(w, r.width)
                        << "n=" << n << " mask=" << mask << " k=" << k;
                    ASSERT_LE(w, k)
                        << "n=" << n << " mask=" << mask << " k=" << k;
                    ASSERT_GE(w, tw)
                        << "n=" << n << " mask=" << mask << " k=" << k;
                }
            }
        }
    }
}

// Complete graphs pin the boundary exactly: tw(K_n) = n-1.
TEST(PartialKTreeCompleteGraphTest, CompleteGraphBoundary) {
    for (int n = 2; n <= 8; ++n) {
        std::vector<std::pair<int, int> > edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                edges.push_back(std::make_pair(u, v));
            }
        }
        Graph g(n, edges);
        EXPECT_FALSE(check_partial_ktree(g, n - 2).is_partial_ktree)
            << "n=" << n;
        PartialKTreeResult r = check_partial_ktree(g, n - 1);
        EXPECT_TRUE(r.is_partial_ktree) << "n=" << n;
        EXPECT_EQ(r.width, n - 1) << "n=" << n;
    }
}

// The empty graph is a partial k-tree for every k; nonempty graphs are
// rejected for negative k.
TEST(PartialKTreeEdgeCaseTest, EmptyGraphAndNegativeK) {
    Graph empty(0, std::vector<std::pair<int, int> >());
    PartialKTreeResult r = check_partial_ktree(empty, 0);
    EXPECT_TRUE(r.is_partial_ktree);
    EXPECT_EQ(r.width, -1);
    r = check_partial_ktree(empty, -1);
    EXPECT_TRUE(r.is_partial_ktree);

    Graph single(1, std::vector<std::pair<int, int> >());
    EXPECT_FALSE(check_partial_ktree(single, -1).is_partial_ktree);
    EXPECT_TRUE(check_partial_ktree(single, 0).is_partial_ktree);
}

}  // namespace
