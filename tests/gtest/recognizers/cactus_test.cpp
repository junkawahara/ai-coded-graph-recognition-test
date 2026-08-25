#include "cactus.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_cactus_obstruction;
using graph_recognition::check_cactus;
using graph_recognition::CactusResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "cactus";

class CactusTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CactusTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CactusResult r = check_cactus(g);
    ASSERT_EQ(r.is_cactus, exp == "YES") << "case=" << stem;
    if (!r.is_cactus) {
        // A disconnected non-cactus still has the offending edge somewhere, so
        // the builder is expected to find one in every NO case.
        Obstruction built = build_cactus_obstruction(g);
        EXPECT_EQ(built.kind, ObstructionKind::TWO_CYCLES_SHARING_EDGE) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CactusTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
