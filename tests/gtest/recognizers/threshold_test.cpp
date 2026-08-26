#include "recognizers/threshold.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::build_threshold_obstruction;
using graph_recognition::check_threshold;
using graph_recognition::ThresholdResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::ThresholdAlgorithm;
using graph_recognition::gtest_utils::verify_threshold_creation_sequence;

namespace {

const char* kDir = "threshold";

class ThresholdTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ThresholdTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ThresholdResult r = check_threshold(g);
    ASSERT_EQ(r.is_threshold, exp == "YES") << "case=" << stem;
    if (r.is_threshold) {
        EXPECT_TRUE(verify_threshold_creation_sequence(g, r.creation_order, r.creation_kind))
            << "case=" << stem;
    }

    ThresholdResult alt = check_threshold(g, ThresholdAlgorithm::DEGREE_SEQUENCE);
    ASSERT_EQ(alt.is_threshold, r.is_threshold) << "case=" << stem;
    if (alt.is_threshold) {
        EXPECT_TRUE(
            verify_threshold_creation_sequence(g, alt.creation_order, alt.creation_kind))
            << "case=" << stem;
    } else {
        // The elimination variant keeps the stuck vertices; the fast one works
        // on the degree sequence and reports nothing.
        EXPECT_TRUE(alt.obstruction.kind == ObstructionKind::TWO_K2 ||
                    alt.obstruction.kind == ObstructionKind::C4 ||
                    alt.obstruction.kind == ObstructionKind::P4)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, alt.obstruction)) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, build_threshold_obstruction(g))) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ThresholdTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
