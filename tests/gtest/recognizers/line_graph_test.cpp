#include "recognizers/line_graph.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_line_graph;
using graph_recognition::LineGraphResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "line_graph";

class LineGraphTest : public ::testing::TestWithParam<std::string> {};

TEST_P(LineGraphTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    LineGraphResult r = check_line_graph(g);
    ASSERT_EQ(r.is_line_graph, exp == "YES") << "case=" << stem;
    if (r.is_line_graph) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_krausz_partition(g, r))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, LineGraphTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
