#include "recognizers/circular_arc.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_circular_arc;
using graph_recognition::CircularArcResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "circular_arc";

class CircularArcTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CircularArcTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    CircularArcResult r = check_circular_arc(g);
    ASSERT_EQ(r.is_circular_arc, exp == "YES") << "case=" << stem;

    // The default MCCONNELL variant returns a model only when it fell back to
    // backtracking (non-Helly inputs); whenever a model comes back it must
    // describe this graph.
    if (r.is_circular_arc && !r.arcs.empty()) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_circular_arc_model(
            g, r.arcs, 2 * g.n, false))
            << "case=" << stem;
    }

    // The backtracking variant always constructs a model, which must
    // describe this graph.
    if (g.n <= 8) {
        CircularArcResult bt = check_circular_arc(
            g, graph_recognition::CircularArcAlgorithm::BACKTRACKING);
        ASSERT_EQ(bt.is_circular_arc, r.is_circular_arc) << "case=" << stem;
        if (bt.is_circular_arc && g.n > 0) {
            EXPECT_TRUE(graph_recognition::gtest_utils::verify_circular_arc_model(
                g, bt.arcs, 2 * g.n, false))
                << "case=" << stem;
        }
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CircularArcTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
