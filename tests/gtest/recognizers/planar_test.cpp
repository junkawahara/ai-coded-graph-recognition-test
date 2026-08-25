#include "planar.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::Obstruction;
using graph_recognition::ObstructionKind;
using graph_recognition::build_planar_obstruction;
using graph_recognition::check_planar;
using graph_recognition::PlanarResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::verify_obstruction;

namespace {

const char* kDir = "planar";

class PlanarTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PlanarTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    PlanarResult r = check_planar(g);
    ASSERT_EQ(r.is_planar, exp == "YES") << "case=" << stem;
    if (!r.is_planar) {
        // Wagner's theorem guarantees one of the two minors exists.
        Obstruction built = build_planar_obstruction(g);
        EXPECT_TRUE(built.kind == ObstructionKind::K5_MINOR ||
                    built.kind == ObstructionKind::K33_MINOR)
            << "case=" << stem;
        EXPECT_TRUE(verify_obstruction(g, built)) << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PlanarTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
