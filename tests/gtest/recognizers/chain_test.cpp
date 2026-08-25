#include "chain.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_chain;
using graph_recognition::ChainResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "chain";

class ChainTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChainTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ChainResult r = check_chain(g);
    ASSERT_EQ(r.is_chain, exp == "YES") << "case=" << stem;
    if (r.is_chain) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_chain_orders(
            g, r.color, r.x_ordering, r.y_ordering, false))
            << "case=" << stem;
    }

    graph_recognition::ChainResult alt =
        check_chain(g, graph_recognition::ChainAlgorithm::NEIGHBORHOOD_INCLUSION);
    ASSERT_EQ(alt.is_chain, r.is_chain) << "case=" << stem;
    if (alt.is_chain) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_chain_orders(
            g, alt.color, alt.x_ordering, alt.y_ordering, false))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChainTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
