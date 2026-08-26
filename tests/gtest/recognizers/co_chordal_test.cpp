#include "recognizers/co_chordal.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_co_chordal;
using graph_recognition::CoChordalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "co_chordal";

class CoChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CoChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CoChordalResult r = check_co_chordal(g);
    ASSERT_EQ(r.is_co_chordal, exp == "YES") << "case=" << stem;
    if (!r.is_co_chordal) {
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::HOLE) << "case=" << stem;
        EXPECT_TRUE(r.obstruction.in_complement) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CoChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
