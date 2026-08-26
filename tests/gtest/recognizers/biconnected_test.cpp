#include "recognizers/biconnected.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::BiconnectedAlgorithm;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_biconnected;
using graph_recognition::BiconnectedResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "biconnected";

class BiconnectedTest : public ::testing::TestWithParam<std::string> {};

TEST_P(BiconnectedTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    const BiconnectedAlgorithm algos[] = {BiconnectedAlgorithm::DFS,
                                          BiconnectedAlgorithm::BLOCK_CUT_TREE};
    for (size_t i = 0; i < 2; ++i) {
        BiconnectedResult r = check_biconnected(g, algos[i]);
        EXPECT_EQ(r.is_biconnected, exp == "YES") << "case=" << stem << " algo=" << i;
        if (r.is_biconnected) continue;
        // Graphs on fewer than 3 vertices fail on size alone and carry no
        // structural witness.
        if (g.n < 3) {
            EXPECT_EQ(r.obstruction.kind, ObstructionKind::NONE)
                << "case=" << stem << " algo=" << i;
            continue;
        }
        EXPECT_TRUE(r.obstruction.kind == ObstructionKind::CUT_VERTEX ||
                    r.obstruction.kind == ObstructionKind::DISCONNECTED_PAIR)
            << "case=" << stem << " algo=" << i;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction))
            << "case=" << stem << " algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, BiconnectedTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
