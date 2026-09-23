#include "recognizers/outer_planar.h"
#include "test_helpers.h"
#include <gtest/gtest.h>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::check_outer_planar;
using graph_recognition::OuterPlanarAlgorithm;
using graph_recognition::OuterPlanarResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "outer_planar";

class OuterPlanarTest : public ::testing::TestWithParam<std::string> {};

TEST_P(OuterPlanarTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    OuterPlanarResult r = check_outer_planar(g);
    ASSERT_EQ(r.is_outer_planar, exp == "YES") << "case=" << stem;

    OuterPlanarResult m = check_outer_planar(g, OuterPlanarAlgorithm::MINOR_CHECK);
    ASSERT_EQ(m.is_outer_planar, exp == "YES") << "case=" << stem << " (MINOR_CHECK)";
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, OuterPlanarTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });


// K_{a,b} on vertices {1..a} x {a+1..a+b}.
Graph complete_bipartite(int a, int b) {
    std::vector<std::pair<int, int> > edges;
    for (int u = 1; u <= a; ++u) {
        for (int v = a + 1; v <= a + b; ++v) edges.push_back(std::make_pair(u, v));
    }
    return Graph(a + b, edges);
}

// The MINOR_CHECK variant rejects a graph through one of two branches, K4 or
// K_{2,3}. Every fixed case above that contains a K_{2,3} minor also contains
// a K4 minor or trips the edge bound first, so the K_{2,3} branch is
// exercised here on K4-free hosts where it is the only reason for NO.
TEST(OuterPlanarMinorCheckTest, K23BranchRejectsK4FreeGraphs) {
    // K_{2,3} itself: 5 vertices, 6 edges, exactly the minimum for the branch.
    EXPECT_FALSE(check_outer_planar(complete_bipartite(2, 3),
                                    OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar);
    // K_{2,4} is series-parallel (no K4 minor), so only the K_{2,3} branch fires.
    EXPECT_FALSE(check_outer_planar(complete_bipartite(2, 4),
                                    OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar);
    // K_{2,3} with one edge subdivided: the minor needs a contraction.
    {
        std::vector<std::pair<int, int> > edges;
        edges.push_back(std::make_pair(1, 3));
        edges.push_back(std::make_pair(1, 4));
        edges.push_back(std::make_pair(1, 5));
        edges.push_back(std::make_pair(2, 3));
        edges.push_back(std::make_pair(2, 4));
        edges.push_back(std::make_pair(2, 6));
        edges.push_back(std::make_pair(6, 5));
        EXPECT_FALSE(check_outer_planar(Graph(6, edges),
                                        OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar);
    }
    // K_{2,2} = C4 stays outerplanar: the branch must not fire below 3 on one side.
    EXPECT_TRUE(check_outer_planar(complete_bipartite(2, 2),
                                   OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar);
    // K_{1,5} (a star) has 6 edges but no K_{2,3} minor.
    EXPECT_TRUE(check_outer_planar(complete_bipartite(1, 5),
                                   OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar);
}

// Both variants must agree on every graph with n <= 6 (2^15 graphs at n = 6).
TEST(OuterPlanarMinorCheckTest, AgreesWithAugmentedPlanarityOnAllSmallGraphs) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int> > pairs;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) pairs.push_back(std::make_pair(u, v));
        }
        for (int mask = 0; mask < (1 << (int)pairs.size()); ++mask) {
            std::vector<std::pair<int, int> > edges;
            for (size_t e = 0; e < pairs.size(); ++e) {
                if (mask & (1 << (int)e)) edges.push_back(pairs[e]);
            }
            Graph g(n, edges);
            const bool lr = check_outer_planar(g).is_outer_planar;
            const bool minor =
                check_outer_planar(g, OuterPlanarAlgorithm::MINOR_CHECK).is_outer_planar;
            ASSERT_EQ(minor, lr) << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
