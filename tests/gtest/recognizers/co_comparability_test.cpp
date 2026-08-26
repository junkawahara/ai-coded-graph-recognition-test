#include "recognizers/co_comparability.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_co_comparability_obstruction;
using graph_recognition::check_co_comparability;
using graph_recognition::CoComparabilityResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "co_comparability";

class CoComparabilityTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CoComparabilityTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CoComparabilityResult r = check_co_comparability(g);
    ASSERT_EQ(r.is_co_comparability, exp == "YES") << "case=" << stem;
    if (!r.is_co_comparability) {
        Obstruction built = build_co_comparability_obstruction(g);
        EXPECT_EQ(built.kind, ObstructionKind::FORCING_CYCLE) << "case=" << stem;
        EXPECT_TRUE(built.in_complement) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CoComparabilityTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
