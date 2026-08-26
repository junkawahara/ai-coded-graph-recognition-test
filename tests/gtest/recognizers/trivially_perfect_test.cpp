#include "recognizers/trivially_perfect.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_trivially_perfect;
using graph_recognition::TriviallyPerfectResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "trivially_perfect";

class TriviallyPerfectTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TriviallyPerfectTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    TriviallyPerfectResult r = check_trivially_perfect(g);
    ASSERT_EQ(r.is_trivially_perfect, exp == "YES") << "case=" << stem;
    if (!r.is_trivially_perfect) {
        EXPECT_TRUE(r.obstruction.kind == ObstructionKind::C4 ||
                    r.obstruction.kind == ObstructionKind::P4)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, TriviallyPerfectTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
