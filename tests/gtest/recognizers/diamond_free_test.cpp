#include "recognizers/diamond_free.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::DiamondFreeAlgorithm;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_diamond_free;
using graph_recognition::DiamondFreeResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "diamond_free";

class DiamondFreeTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DiamondFreeTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    const DiamondFreeAlgorithm algos[] = {DiamondFreeAlgorithm::BRUTE,
                                          DiamondFreeAlgorithm::EDGE_PAIR};
    for (size_t i = 0; i < 2; ++i) {
        DiamondFreeResult r = check_diamond_free(g, algos[i]);
        ASSERT_EQ(r.is_diamond_free, exp == "YES") << "case=" << stem << " algo=" << i;
        if (r.is_diamond_free) continue;
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::DIAMOND)
            << "case=" << stem << " algo=" << i;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction))
            << "case=" << stem << " algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DiamondFreeTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
