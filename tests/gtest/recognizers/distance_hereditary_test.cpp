#include "recognizers/distance_hereditary.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_distance_hereditary_obstruction;
using graph_recognition::check_distance_hereditary;
using graph_recognition::DistanceHereditaryResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "distance_hereditary";

class DistanceHereditaryTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DistanceHereditaryTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    DistanceHereditaryResult r = check_distance_hereditary(g);
    ASSERT_EQ(r.is_distance_hereditary, exp == "YES") << "case=" << stem;
    if (!r.is_distance_hereditary) {
        Obstruction built = build_distance_hereditary_obstruction(g);
        EXPECT_EQ(built.kind, ObstructionKind::NON_SHORTEST_INDUCED_PATH) << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DistanceHereditaryTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
