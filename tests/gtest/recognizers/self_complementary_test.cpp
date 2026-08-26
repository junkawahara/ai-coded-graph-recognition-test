#include "recognizers/self_complementary.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_self_complementary;
using graph_recognition::SelfComplementaryResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "self_complementary";

class SelfComplementaryTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SelfComplementaryTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    SelfComplementaryResult r = check_self_complementary(g);
    ASSERT_EQ(r.is_self_complementary, exp == "YES") << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SelfComplementaryTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
