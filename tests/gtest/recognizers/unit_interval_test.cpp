#include "unit_interval.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_unit_interval;
using graph_recognition::UnitIntervalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "unit_interval";

class UnitIntervalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(UnitIntervalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    UnitIntervalResult r = check_unit_interval(g);
    ASSERT_EQ(r.is_unit_interval, exp == "YES") << "case=" << stem;
    if (!r.is_unit_interval) {
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, UnitIntervalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
