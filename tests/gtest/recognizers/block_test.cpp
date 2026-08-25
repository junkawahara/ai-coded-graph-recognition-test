#include "block.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::BlockAlgorithm;
using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_block_obstruction;
using graph_recognition::check_block;
using graph_recognition::BlockResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "block";

class BlockTest : public ::testing::TestWithParam<std::string> {};

TEST_P(BlockTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    BlockResult r = check_block(g);
    ASSERT_EQ(r.is_block, exp == "YES") << "case=" << stem;
    if (r.is_block) return;

    // The default DFS variant reports no witness; the builder and the
    // characterization-based variant both have to produce a valid one.
    Obstruction built = build_block_obstruction(g);
    EXPECT_TRUE(built.kind == ObstructionKind::HOLE ||
                built.kind == ObstructionKind::DIAMOND)
        << "case=" << stem;
    EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;

    BlockResult cdf = check_block(g, BlockAlgorithm::CHORDAL_DIAMOND_FREE);
    ASSERT_FALSE(cdf.is_block) << "case=" << stem;
    EXPECT_TRUE(verify_obstruction(g, cdf.obstruction)) << "case=" << stem;
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, BlockTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
