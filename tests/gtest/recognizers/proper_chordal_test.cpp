#include "proper_chordal.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_proper_chordal;
using graph_recognition::ProperChordalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "proper_chordal";

class ProperChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ProperChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ProperChordalResult r = check_proper_chordal(g);
    EXPECT_EQ(r.is_proper_chordal, exp == "YES") << "case=" << stem;
    if (r.is_proper_chordal) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_tree_layout(g, r.layout_parent))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ProperChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
