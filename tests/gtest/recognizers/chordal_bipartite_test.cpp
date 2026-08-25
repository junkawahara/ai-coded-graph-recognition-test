#include "chordal_bipartite.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::ChordalBipartiteAlgorithm;
using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::check_chordal_bipartite;
using graph_recognition::ChordalBipartiteResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;
using graph_recognition::gtest_utils::verify_bipartite_coloring;

namespace {

const char* kDir = "chordal_bipartite";

class ChordalBipartiteTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalBipartiteTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ChordalBipartiteResult r = check_chordal_bipartite(g);
    ASSERT_EQ(r.is_chordal_bipartite, exp == "YES") << "case=" << stem;
    if (r.is_chordal_bipartite) {
        EXPECT_TRUE(verify_bipartite_coloring(g, r.color)) << "case=" << stem;
    } else {
        EXPECT_TRUE(r.obstruction.kind == ObstructionKind::ODD_CYCLE ||
                    r.obstruction.kind == ObstructionKind::EVEN_HOLE)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalBipartiteTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
