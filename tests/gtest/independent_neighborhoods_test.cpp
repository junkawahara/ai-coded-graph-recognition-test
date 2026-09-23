#include "util/independent_neighborhoods.h"
#include "recognizers/triangle_free.h"
#include <gtest/gtest.h>

#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::check_triangle_free;
using graph_recognition::detail::enumerate_independent_neighborhoods;

// Recognize every possible child independently of the neighborhood generator.
// This covers empty, disconnected, bipartite and odd-cycle parents.
TEST(IndependentNeighborhoodsTest, MatchesRecognitionExhaustivelyInLegacyOrder) {
    for (int k = 0; k <= 5; ++k) {
        const int pairs = k * (k - 1) / 2;
        for (unsigned long long mask = 0; mask < (1ULL << pairs); ++mask) {
            std::vector<std::pair<int, int>> edges;
            std::vector<unsigned long long> adj(k, 0);
            int bit = 0;
            for (int u = 0; u < k; ++u) {
                for (int v = u + 1; v < k; ++v, ++bit) {
                    if (!((mask >> bit) & 1ULL)) continue;
                    edges.emplace_back(u + 1, v + 1);
                    adj[u] |= 1ULL << v;
                    adj[v] |= 1ULL << u;
                }
            }
            if (!check_triangle_free(Graph(k, edges)).is_triangle_free) continue;
            SCOPED_TRACE(::testing::Message() << "k=" << k << " parent=" << mask);

            std::vector<unsigned long long> expected, actual;
            for (unsigned long long s = 0; s < (1ULL << k); ++s) {
                auto child = edges;
                for (int u = 0; u < k; ++u)
                    if ((s >> u) & 1ULL) child.emplace_back(u + 1, k + 1);
                if (check_triangle_free(Graph(k + 1, child)).is_triangle_free)
                    expected.push_back(s);
            }
            enumerate_independent_neighborhoods(k, adj,
                [&](unsigned long long s) { actual.push_back(s); });
            EXPECT_EQ(actual, expected);
        }
    }
}

TEST(IndependentNeighborhoodsTest, DenseParentGeneratesOnlyIndependentChoices) {
    // K16,16 has 2^17 - 1 independent sets, versus 2^32 arbitrary subsets.
    const unsigned long long left = (1ULL << 16) - 1;
    const unsigned long long right = left << 16;
    std::vector<unsigned long long> adj(32);
    for (int u = 0; u < 32; ++u) adj[u] = u < 16 ? right : left;
    std::size_t count = 0;
    unsigned long long previous = 0;
    enumerate_independent_neighborhoods(32, adj, [&](unsigned long long s) {
        EXPECT_TRUE((s & left) == 0 || (s & right) == 0);
        if (count != 0) {
            EXPECT_GT(s, previous);
        }
        previous = s;
        ++count;
    });
    EXPECT_EQ(count, (1u << 17) - 1);
}

TEST(IndependentNeighborhoodsTest, HandlesTheHighestBits) {
    // A clique keeps output small while exercising masks up to bit 63.
    // The utility works for any simple undirected parent.
    for (int k : {63, 64}) {
        const unsigned long long all = k == 64 ? ~0ULL : (1ULL << k) - 1;
        std::vector<unsigned long long> adj(k);
        std::vector<unsigned long long> expected(1, 0), actual;
        for (int u = 0; u < k; ++u) {
            adj[u] = all & ~(1ULL << u);
            expected.push_back(1ULL << u);
        }
        enumerate_independent_neighborhoods(k, adj,
            [&](unsigned long long s) { actual.push_back(s); });
        EXPECT_EQ(actual, expected);
    }
}

}  // namespace
