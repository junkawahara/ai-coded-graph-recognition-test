#include "util/bipartite_neighborhoods.h"
#include "recognizers/bipartite.h"
#include <gtest/gtest.h>

#include <set>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::check_bipartite;
using graph_recognition::detail::BipartiteComponentSides;
using graph_recognition::detail::bipartite_component_sides;
using graph_recognition::detail::enumerate_bipartite_neighborhoods;

// Independent oracle: recognize every possible child of every small bipartite
// parent, including isolates and disconnected parents needing color flips.
TEST(BipartiteNeighborhoodsTest, MatchesRecognitionExhaustively) {
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
            if (!check_bipartite(Graph(k, edges)).is_bipartite) continue;
            SCOPED_TRACE(::testing::Message() << "k=" << k << " parent=" << mask);

            std::set<unsigned long long> expected, actual;
            for (unsigned long long s = 0; s < (1ULL << k); ++s) {
                auto child = edges;
                for (int u = 0; u < k; ++u)
                    if ((s >> u) & 1ULL) child.emplace_back(u + 1, k + 1);
                if (check_bipartite(Graph(k + 1, child)).is_bipartite)
                    expected.insert(s);
            }
            enumerate_bipartite_neighborhoods(bipartite_component_sides(k, adj),
                [&](unsigned long long s) { EXPECT_TRUE(actual.insert(s).second); });
            EXPECT_EQ(actual, expected);
        }
    }
}

TEST(BipartiteNeighborhoodsTest, JoinsIndependentlyColoredComponentsAndAnIsolate) {
    // Edges 0-1 and 2-3, plus isolated vertex 4: 3 * 3 * 2 choices.
    const std::vector<unsigned long long> adj = {2, 1, 8, 4, 0};
    std::set<unsigned long long> masks;
    enumerate_bipartite_neighborhoods(bipartite_component_sides(5, adj),
        [&](unsigned long long s) { EXPECT_TRUE(masks.insert(s).second); });
    EXPECT_EQ(masks.size(), 18u);
    EXPECT_EQ(masks.count(0), 1u);
    EXPECT_EQ(masks.count(1 | 8 | 16), 1u);  // opposite named sides in the two components
    EXPECT_EQ(masks.count(1 | 2), 0u);       // would create a triangle
}

TEST(BipartiteNeighborhoodsTest, DenseParentGeneratesOnlyPermittedChoices) {
    // K16,16 has 2^17 - 1 valid neighborhoods, versus 2^32 arbitrary subsets.
    const unsigned long long left = (1ULL << 16) - 1;
    const unsigned long long right = left << 16;
    std::vector<unsigned long long> adj(32);
    for (int u = 0; u < 32; ++u) adj[u] = u < 16 ? right : left;
    std::size_t count = 0;
    bool valid = true;
    enumerate_bipartite_neighborhoods(bipartite_component_sides(32, adj),
        [&](unsigned long long s) {
            ++count;
            valid = valid && ((s & left) == 0 || (s & right) == 0);
        });
    EXPECT_TRUE(valid);
    EXPECT_EQ(count, (1u << 17) - 1);
}

TEST(BipartiteNeighborhoodsTest, HandlesTheHighestBitAndSparseMasks) {
    const unsigned long long high = 1ULL << 63;
    const unsigned long long low = 1ULL << 40;
    const std::vector<BipartiteComponentSides> sides = {{high, 1}, {low, 0}};
    std::set<unsigned long long> actual;
    enumerate_bipartite_neighborhoods(sides,
        [&](unsigned long long s) { EXPECT_TRUE(actual.insert(s).second); });
    const std::set<unsigned long long> expected = {0, high, 1, low, high | low, 1 | low};
    EXPECT_EQ(actual, expected);

    std::vector<unsigned long long> adj(64, 0);
    adj[0] = high;
    adj[63] = 1;
    const auto components = bipartite_component_sides(64, adj);
    ASSERT_EQ(components.size(), 63u);
    EXPECT_EQ(components[0].first, 1u);
    EXPECT_EQ(components[0].second, high);
}

}  // namespace
