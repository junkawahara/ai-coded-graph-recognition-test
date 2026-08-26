#include "recognizers/weakly_chordal.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::ObstructionKind;
using graph_recognition::WeaklyChordalAlgorithm;
using graph_recognition::check_weakly_chordal;
using graph_recognition::WeaklyChordalResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "weakly_chordal";

class WeaklyChordalTest : public ::testing::TestWithParam<std::string> {};

TEST_P(WeaklyChordalTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    const WeaklyChordalAlgorithm algos[] = {
        WeaklyChordalAlgorithm::CO_CHORDAL_BIPARTITE,
        WeaklyChordalAlgorithm::COMPLEMENT_BFS};
    for (size_t i = 0; i < 2; ++i) {
        WeaklyChordalResult r = check_weakly_chordal(g, algos[i]);
        ASSERT_EQ(r.is_weakly_chordal, exp == "YES") << "case=" << stem << " algo=" << i;
        if (r.is_weakly_chordal) continue;
        EXPECT_EQ(r.obstruction.kind, ObstructionKind::HOLE)
            << "case=" << stem << " algo=" << i;
        EXPECT_GE(r.obstruction.vertices.size(), 5u) << "case=" << stem << " algo=" << i;
        EXPECT_TRUE(verify_obstruction(g, r.obstruction))
            << "case=" << stem << " algo=" << i;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, WeaklyChordalTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
