#include "recognizers/three_leaf_power.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_three_leaf_power;
using graph_recognition::ThreeLeafPowerResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "three_leaf_power";

class ThreeLeafPowerTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ThreeLeafPowerTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ThreeLeafPowerResult r = check_three_leaf_power(g);
    ASSERT_EQ(r.is_three_leaf_power, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ThreeLeafPowerTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
