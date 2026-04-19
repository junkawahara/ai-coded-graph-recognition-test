#include "interval.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::IntervalAlgorithm;
using graph_recognition::IntervalResult;
using graph_recognition::check_interval;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_interval_model;

namespace {

const char* kDir = "interval";

class IntervalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(IntervalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    IntervalResult r = check_interval(g);
    ASSERT_EQ(r.is_interval, exp == "YES") << "case=" << stem;
    if (r.is_interval) {
        EXPECT_TRUE(verify_interval_model(g, r.intervals)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, IntervalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

class IntervalVariantTest : public ::testing::TestWithParam<std::string> {};

TEST_P(IntervalVariantTest, BacktrackingAgreesWithAtFree) {
    Graph g = load_graph(test_path(std::string(kDir) + "/" + GetParam() + ".in"));
    bool bt = check_interval(g, IntervalAlgorithm::BACKTRACKING).is_interval;
    bool af = check_interval(g, IntervalAlgorithm::AT_FREE).is_interval;
    EXPECT_EQ(bt, af) << "case=" << GetParam();
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, IntervalVariantTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
