#include "recognizers/proper_circular_arc.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_proper_circular_arc;
using graph_recognition::ProperCircularArcResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "proper_circular_arc";

class ProperCircularArcTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ProperCircularArcTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    ProperCircularArcResult r = check_proper_circular_arc(g);
    ASSERT_EQ(r.is_proper_circular_arc, exp == "YES") << "case=" << stem;
    if (r.is_proper_circular_arc && g.n > 0) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_circular_arc_model(
            g, r.arcs, 2 * g.n, true))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ProperCircularArcTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
