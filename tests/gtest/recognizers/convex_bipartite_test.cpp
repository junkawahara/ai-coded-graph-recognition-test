#include "recognizers/convex_bipartite.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_convex_bipartite;
using graph_recognition::ConvexBipartiteResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_convex_bipartite;

namespace {

const char* kDir = "convex_bipartite";

class ConvexBipartiteTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ConvexBipartiteTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ConvexBipartiteResult r = check_convex_bipartite(g);
    ASSERT_EQ(r.is_convex_bipartite, exp == "YES") << "case=" << stem;
    if (r.is_convex_bipartite) {
        EXPECT_TRUE(verify_convex_bipartite(g, r.color, r.ordering)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ConvexBipartiteTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
