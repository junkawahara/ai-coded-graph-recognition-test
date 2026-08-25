#include "proper_interval.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::build_proper_interval_obstruction;
using graph_recognition::check_proper_interval;
using graph_recognition::ProperIntervalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "proper_interval";

class ProperIntervalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ProperIntervalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ProperIntervalResult r = check_proper_interval(g);
    ASSERT_EQ(r.is_proper_interval, exp == "YES") << "case=" << stem;
    if (r.is_proper_interval) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_indifference_order(
            g, r.indifference_order, r.number))
            << "case=" << stem;
    } else {
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
        Obstruction built = build_proper_interval_obstruction(g);
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ProperIntervalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
