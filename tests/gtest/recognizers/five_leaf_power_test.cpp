#include "five_leaf_power.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::check_five_leaf_power;
using graph_recognition::FiveLeafPowerResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "five_leaf_power";

class FiveLeafPowerTest : public ::testing::TestWithParam<std::string> {};

TEST_P(FiveLeafPowerTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    FiveLeafPowerResult r = check_five_leaf_power(g);
    ASSERT_EQ(r.is_five_leaf_power, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, FiveLeafPowerTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Regression for the Steiner-branch extension of the 3-Steiner root
// search: three "arms" of four nodes each (tree distances 1..4 from a
// common hub) with the cross-arm adjacencies induced by distance <= 3.
// The hub position is not occupiable by any quotient node, so the
// quotient is realizable with one Steiner branch node but NOT with the
// quotient nodes alone (the pre-2026-08 search answered NO here).
TEST(FiveLeafPowerSteinerTest, HubQuotientNeedsSteinerNode) {
    const int k = 12;
    std::vector<std::vector<char>> Q(k, std::vector<char>(k, 0));
    for (int i = 0; i < k; ++i) Q[i][i] = 1;
    auto add = [&Q](int a, int b) { Q[a][b] = Q[b][a] = 1; };
    for (int a = 0; a < 3; ++a) {
        int p = 4 * a, q = 4 * a + 1, r = 4 * a + 2, t = 4 * a + 3;
        add(p, q); add(q, r); add(r, t);   // consecutive arm nodes
        add(p, r); add(q, t); add(p, t);   // arm distances 2 and 3
        for (int b = 0; b < 3; ++b) {
            if (a < b) add(p, 4 * b);       // p_a ~ p_b (distance 2)
            if (a != b) add(p, 4 * b + 1);  // p_a ~ q_b (distance 3)
        }
    }

    graph_recognition::detail_five_leaf_power::RealizationSearch search;
    search.init(Q, k, 0);
    EXPECT_FALSE(search.run()) << "realizable without a Steiner node?";
    search.init(Q, k, 1);
    EXPECT_TRUE(search.run()) << "one Steiner hub should realize this";
    EXPECT_TRUE(graph_recognition::detail_five_leaf_power::quotient_realizable(Q, k));
}

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

// Every path is a 5-leaf power: its critical-clique quotient is the path
// itself, realized by giving every quotient edge weight 2 (adjacent pairs
// at distance 2 <= 3, distance-2 pairs at 4 >= 4). A former 64-bit
// path-mask guard silently answered NO for any quotient component with
// more than 64 nodes, so P_65 was the first wrong answer; cover the cliff.
TEST(FiveLeafPowerLargeTest, LongPathsAreFiveLeafPowers) {
    const int sizes[] = {63, 64, 65, 66, 80};
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); ++i) {
        int n = sizes[i];
        FiveLeafPowerResult r = check_five_leaf_power(make_path(n));
        EXPECT_TRUE(r.is_five_leaf_power) << "P_" << n;
    }
}

TEST(FiveLeafPowerBudgetTest, ExhaustedBudgetThrowsInsteadOfAnsweringNo) {
    // P_30 needs about 30 growth steps just to build its quotient spanning
    // tree, so a budget of 10 must abort -- and the abort must surface as
    // an exception, never as a silent NO.
    Graph g = make_path(30);
    EXPECT_THROW(check_five_leaf_power(g, 10ULL), std::runtime_error);
    // With the default budget the same instance is decided.
    EXPECT_TRUE(check_five_leaf_power(g).is_five_leaf_power);
}

}  // namespace
