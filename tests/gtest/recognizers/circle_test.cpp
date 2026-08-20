#include "circle.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <stdexcept>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::check_circle;
using graph_recognition::CircleAlgorithm;
using graph_recognition::CircleResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_circle_dow;

namespace {

const char* kDir = "circle";

class CircleTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CircleTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    // Default algorithm (Naji's linear system): decision only.
    CircleResult r = check_circle(g);
    ASSERT_EQ(r.is_circle, exp == "YES") << "case=" << stem;
    EXPECT_TRUE(r.dow.empty()) << "case=" << stem;

    // DOW backtracking is exponential in the worst case (a NO answer at n = 8
    // already costs ~1.3 s), so exercise it (and its certificate) only on
    // small instances; CircleProperty covers random n = 7, 8 agreement.
    if (g.n <= 7) {
        CircleResult rd = check_circle(g, CircleAlgorithm::DOW_BACKTRACKING);
        ASSERT_EQ(rd.is_circle, r.is_circle) << "case=" << stem;
        if (rd.is_circle) {
            EXPECT_TRUE(verify_circle_dow(g, rd.dow)) << "case=" << stem;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CircleTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Twin contraction: circle graphs are closed under adding/removing both
// true and false twins, so the Naji path contracts twin classes first.
TEST(CircleTwinTest, ContractTwinsCollapsesStarsAndCliques) {
    using graph_recognition::detail_circle::contract_twins;

    // Star K_{1,9}: the 9 leaves are mutual false twins -> K_2, whose two
    // vertices are true twins of each other -> K_1 at the fixpoint.
    std::vector<std::pair<int, int>> star_edges;
    for (int v = 2; v <= 10; ++v) star_edges.push_back(std::make_pair(1, v));
    Graph reduced_star = contract_twins(Graph(10, star_edges));
    EXPECT_EQ(reduced_star.n, 1);

    // K_5: all vertices are mutual true twins -> K_1.
    std::vector<std::pair<int, int>> k5_edges;
    for (int u = 1; u <= 5; ++u)
        for (int v = u + 1; v <= 5; ++v) k5_edges.push_back(std::make_pair(u, v));
    Graph reduced_k5 = contract_twins(Graph(5, k5_edges));
    EXPECT_EQ(reduced_k5.n, 1);

    // P_5 has no twins and must stay untouched.
    std::vector<std::pair<int, int>> p5_edges;
    for (int v = 1; v < 5; ++v) p5_edges.push_back(std::make_pair(v, v + 1));
    Graph reduced_p5 = contract_twins(Graph(5, p5_edges));
    EXPECT_EQ(reduced_p5.n, 5);
}

TEST(CircleTwinTest, LargeStarIsRecognizedInstantly) {
    // K_{1,499} is trivially a circle graph, but its raw Naji system has
    // (n-1)^2 variables and used to die on allocation; twin contraction
    // collapses it to a single edge.
    const int n = 500;
    std::vector<std::pair<int, int>> edges;
    for (int v = 2; v <= n; ++v) edges.push_back(std::make_pair(1, v));
    CircleResult r = check_circle(Graph(n, edges));
    EXPECT_TRUE(r.is_circle);
}

TEST(CircleLimitTest, NajiMemoryLimitThrowsInsteadOfOom) {
    // C_6 yields a Naji system with several independent rows; a 1-word
    // basis limit must surface as an exception, never as a wrong answer.
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v <= 6; ++v) edges.push_back(std::make_pair(v, v % 6 + 1));
    Graph g(6, edges);
    EXPECT_THROW(
        graph_recognition::detail_circle::check_circle_naji_system(g, 1),
        std::runtime_error);
    // The default limit decides the same instance (C_6 is a circle graph).
    EXPECT_TRUE(check_circle(g).is_circle);
}

TEST(CircleLimitTest, DowBudgetThrowsInsteadOfHanging) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    Graph g(5, edges);
    EXPECT_THROW(graph_recognition::detail_circle::check_circle_dow(g, 1),
                 std::runtime_error);
    // With the default budget the same instance is decided (with a DOW).
    CircleResult r = check_circle(g, CircleAlgorithm::DOW_BACKTRACKING);
    EXPECT_TRUE(r.is_circle);
    EXPECT_EQ(r.dow.size(), 10u);
}

}  // namespace
