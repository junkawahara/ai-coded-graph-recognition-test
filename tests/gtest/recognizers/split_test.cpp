#include "recognizers/split.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::build_split_obstruction;
using graph_recognition::check_split;
using graph_recognition::SplitResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::SplitAlgorithm;
using graph_recognition::gtest_utils::verify_split_partition;

namespace {

const char* kDir = "split";

class SplitTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SplitTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    SplitResult r = check_split(g);
    ASSERT_EQ(r.is_split, exp == "YES") << "case=" << stem;
    if (r.is_split) {
        EXPECT_TRUE(verify_split_partition(g, r.side)) << "case=" << stem;
    }

    // Both variants agree, and both report a usable partition.
    SplitResult alt = check_split(g, SplitAlgorithm::DEGREE_SEQUENCE);
    ASSERT_EQ(alt.is_split, r.is_split) << "case=" << stem;
    if (alt.is_split) {
        EXPECT_TRUE(verify_split_partition(g, alt.side)) << "case=" << stem;
    } else {
        // Split graphs are the {2K2, C4, C5}-free graphs, so the projection of
        // a long hole has to land on one of the three.
        EXPECT_TRUE(alt.obstruction.kind == ObstructionKind::TWO_K2 ||
                    alt.obstruction.kind == ObstructionKind::C4 ||
                    alt.obstruction.kind == ObstructionKind::C5)
            << "case=" << stem;
        EXPECT_FALSE(alt.obstruction.in_complement) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, alt.obstruction)) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, build_split_obstruction(g))) << "case=" << stem;
    }
}

// The empty graph is a split graph, so both variants owe the caller the
// documented size n+1 partition rather than an empty vector.
TEST(SplitEmptyGraphTest, BothVariantsReturnAUsablePartition) {
    Graph g(0, std::vector<std::pair<int, int>>());
    const SplitAlgorithm algos[2] = {SplitAlgorithm::HAMMER_SIMEONE,
                                     SplitAlgorithm::DEGREE_SEQUENCE};
    for (int i = 0; i < 2; ++i) {
        SplitResult r = check_split(g, algos[i]);
        EXPECT_TRUE(r.is_split) << "algo=" << i;
        EXPECT_EQ(r.side.size(), 1u) << "algo=" << i;
        EXPECT_TRUE(verify_split_partition(g, r.side)) << "algo=" << i;
    }
}

// A value outside the enum must not read as a mathematical NO (which would
// come with neither a partition nor an obstruction).
TEST(SplitAlgorithmSelectorTest, OutOfRangeValueRunsTheDefault) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    Graph g(3, edges);
    SplitResult bogus = check_split(g, static_cast<SplitAlgorithm>(99));
    SplitResult def = check_split(g);
    EXPECT_TRUE(bogus.is_split);
    EXPECT_EQ(bogus.side, def.side);
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SplitTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
