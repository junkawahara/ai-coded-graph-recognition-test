#include "recognizers/maximal_outer_planar.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_maximal_outer_planar;
using graph_recognition::MaximalOuterPlanarResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "maximal_outer_planar";

class MaximalOuterPlanarTest : public ::testing::TestWithParam<std::string> {};

TEST_P(MaximalOuterPlanarTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    MaximalOuterPlanarResult r = check_maximal_outer_planar(g);
    ASSERT_EQ(r.is_maximal_outer_planar, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, MaximalOuterPlanarTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
