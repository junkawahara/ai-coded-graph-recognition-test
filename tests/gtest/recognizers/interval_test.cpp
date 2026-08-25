#include "interval.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_interval_obstruction;
using graph_recognition::IntervalAlgorithm;
using graph_recognition::IntervalResult;
using graph_recognition::check_interval;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::gtest_utils::verify_interval_model;

namespace {

const char* kDir = "interval";

class IntervalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(IntervalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    IntervalResult r = check_interval(g);
    ASSERT_EQ(r.is_interval, exp == "YES") << "case=" << stem;
    if (r.is_interval) {
        EXPECT_TRUE(verify_interval_model(g, r.intervals)) << "case=" << stem;
    } else {
        // The default AT_FREE variant always has a witness in hand.
        EXPECT_TRUE(r.obstruction.kind == ObstructionKind::HOLE ||
                    r.obstruction.kind == ObstructionKind::ASTEROIDAL_TRIPLE)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;

        Obstruction built = build_interval_obstruction(g);
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, IntervalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

class IntervalVariantTest : public ::testing::TestWithParam<std::string> {};

TEST_P(IntervalVariantTest, AllAlgorithmsAgree) {
    Graph g = load_graph(test_path(std::string(kDir) + "/" + GetParam() + ".in"));
    IntervalResult bt = check_interval(g, IntervalAlgorithm::BACKTRACKING);
    IntervalResult af = check_interval(g, IntervalAlgorithm::AT_FREE);
    IntervalResult pq = check_interval(g, IntervalAlgorithm::PQ_TREE);
    EXPECT_EQ(bt.is_interval, af.is_interval) << "case=" << GetParam();
    EXPECT_EQ(bt.is_interval, pq.is_interval) << "case=" << GetParam();
    // The three searches report different orders of the same clique path, so
    // each model is verified on its own rather than compared to the others.
    if (bt.is_interval) {
        EXPECT_TRUE(verify_interval_model(g, bt.intervals)) << "case=" << GetParam();
        EXPECT_TRUE(verify_interval_model(g, af.intervals)) << "case=" << GetParam();
        EXPECT_TRUE(verify_interval_model(g, pq.intervals)) << "case=" << GetParam();
    } else {
        // Only AT_FREE is required to produce a witness; the other two report
        // one exactly when chordality is what failed.
        EXPECT_TRUE(verify_obstruction(g, af.obstruction)) << "case=" << GetParam();
        if (bt.obstruction.has_witness()) {
            EXPECT_TRUE(verify_obstruction(g, bt.obstruction)) << "case=" << GetParam();
        }
        if (pq.obstruction.has_witness()) {
            EXPECT_TRUE(verify_obstruction(g, pq.obstruction)) << "case=" << GetParam();
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, IntervalVariantTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
