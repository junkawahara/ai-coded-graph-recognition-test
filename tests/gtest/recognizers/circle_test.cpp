#include "circle.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

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

}  // namespace
