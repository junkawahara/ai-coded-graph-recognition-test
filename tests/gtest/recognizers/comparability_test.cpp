#include "comparability.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_comparability_obstruction;
using graph_recognition::check_comparability;
using graph_recognition::ComparabilityResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::TransitiveOrientationResult;
using graph_recognition::transitive_orientation;
using graph_recognition::gtest_utils::verify_transitive_orientation;

namespace {

const char* kDir = "comparability";

class ComparabilityTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ComparabilityTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ComparabilityResult r = check_comparability(g);
    ASSERT_EQ(r.is_comparability, exp == "YES") << "case=" << stem;
    if (!r.is_comparability) {
        Obstruction built = build_comparability_obstruction(g);
        EXPECT_EQ(built.kind, ObstructionKind::FORCING_CYCLE) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
    if (r.is_comparability) {
        // The orientation the recognizer reports must really be a transitive
        // orientation of this graph.
        TransitiveOrientationResult to = transitive_orientation(g);
        ASSERT_TRUE(to.is_comparability) << "case=" << stem;
        EXPECT_EQ(to.orientation, r.orientation) << "case=" << stem;
        EXPECT_TRUE(verify_transitive_orientation(g, to)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ComparabilityTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
