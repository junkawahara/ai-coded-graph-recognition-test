#include "cochain.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_cochain;
using graph_recognition::CochainResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "cochain";

class CochainTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CochainTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CochainResult r = check_cochain(g);
    ASSERT_EQ(r.is_cochain, exp == "YES") << "case=" << stem;
    if (r.is_cochain) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_chain_orders(
            g, r.color, r.x_ordering, r.y_ordering, true))
            << "case=" << stem;
    }

    graph_recognition::CochainResult alt =
        check_cochain(g, graph_recognition::CochainAlgorithm::COMPLEMENT);
    ASSERT_EQ(alt.is_cochain, r.is_cochain) << "case=" << stem;
    if (alt.is_cochain) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_chain_orders(
            g, alt.color, alt.x_ordering, alt.y_ordering, true))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CochainTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
