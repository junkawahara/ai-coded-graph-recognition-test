#include "series_parallel.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_series_parallel;
using graph_recognition::SeriesParallelResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "series_parallel";

class SeriesParallelTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SeriesParallelTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    SeriesParallelResult r = check_series_parallel(g);
    ASSERT_EQ(r.is_series_parallel, exp == "YES") << "case=" << stem;
    if (r.is_series_parallel) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_sp_reduction(g, r.reductions))
            << "case=" << stem;
    }

    SeriesParallelResult alt = check_series_parallel(
        g, graph_recognition::SeriesParallelAlgorithm::MINOR_CHECK);
    ASSERT_EQ(alt.is_series_parallel, r.is_series_parallel) << "case=" << stem;
    if (alt.is_series_parallel) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_sp_reduction(g, alt.reductions))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SeriesParallelTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
