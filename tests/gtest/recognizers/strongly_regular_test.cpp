#include "strongly_regular.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_strongly_regular;
using graph_recognition::StronglyRegularResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_strongly_regular_params;

namespace {

const char* kDir = "strongly_regular";

class StronglyRegularTest : public ::testing::TestWithParam<std::string> {};

TEST_P(StronglyRegularTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    StronglyRegularResult r = check_strongly_regular(g);
    ASSERT_EQ(r.is_strongly_regular, exp == "YES") << "case=" << stem;
    if (r.is_strongly_regular) {
        EXPECT_TRUE(verify_strongly_regular_params(g, r.k, r.lambda, r.mu)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, StronglyRegularTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
