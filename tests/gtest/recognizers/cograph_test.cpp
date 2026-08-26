#include "recognizers/cograph.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::CographAlgorithm;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_cograph;
using graph_recognition::CographResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "cograph";

class CographTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CographTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    const CographAlgorithm algos[] = {CographAlgorithm::COTREE,
                                      CographAlgorithm::PARTITION_REFINEMENT,
                                      CographAlgorithm::MODULAR};
    for (size_t i = 0; i < 3; ++i) {
        CographResult r = check_cograph(g, algos[i]);
        ASSERT_EQ(r.is_cograph, exp == "YES") << "case=" << stem << " algo=" << i;
        if (r.is_cograph) continue;
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::P4)
            << "case=" << stem << " algo=" << i;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction))
            << "case=" << stem << " algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CographTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
