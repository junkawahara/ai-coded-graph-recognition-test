#include "recognizers/line_graph.h"
#include "certificates.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

using graph_recognition::Graph;
using graph_recognition::check_line_graph;
using graph_recognition::LineGraphResult;
using graph_recognition::gtest_utils::load_graph;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "line_graph";

class LineGraphTest : public ::testing::TestWithParam<std::string> {};

TEST_P(LineGraphTest, MatchesExpected) {
    const std::string stem = GetParam();
    Graph g = load_graph(test_path(std::string(kDir) + "/" + stem + ".in"));
    std::string exp = read_expected(test_path(std::string(kDir) + "/" + stem + ".exp"));

    LineGraphResult r = check_line_graph(g);
    ASSERT_EQ(r.is_line_graph, exp == "YES") << "case=" << stem;
    if (r.is_line_graph) {
        EXPECT_TRUE(graph_recognition::gtest_utils::verify_krausz_partition(g, r))
            << "case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, LineGraphTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// L(H) for H = a star K_{1,s} plus the edge between two of its leaves: K_s on
// the star's edges plus one vertex adjacent to exactly two of them. With
// s >= 30 the common neighbourhood of the first edge is not a clique, and the
// old bit-mask branch answered NO on that alone (review_all_1.md #1).
// The order of the extra vertex's edges changes which edge the search sees
// first, so both placements are covered.
Graph star_plus_leaf_edge_line_graph(int s, bool extra_first) {
    std::vector<std::pair<int, int>> edges;
    if (extra_first) {
        edges.push_back(std::make_pair(1, s + 1));
        edges.push_back(std::make_pair(2, s + 1));
    }
    for (int u = 1; u <= s; ++u)
        for (int v = u + 1; v <= s; ++v) edges.push_back(std::make_pair(u, v));
    if (!extra_first) {
        edges.push_back(std::make_pair(1, s + 1));
        edges.push_back(std::make_pair(2, s + 1));
    }
    return Graph(s + 1, edges);
}

TEST(LineGraphRegressionTest, LargeStarPlusLeafEdgeIsALineGraph) {
    const int sizes[] = {20, 31, 32, 40};
    const graph_recognition::LineGraphAlgorithm algos[] = {
        graph_recognition::LineGraphAlgorithm::BRUTE,
        graph_recognition::LineGraphAlgorithm::KRAUSZ};
    for (size_t si = 0; si < sizeof(sizes) / sizeof(sizes[0]); ++si) {
        for (int extra_first = 0; extra_first < 2; ++extra_first) {
            Graph g = star_plus_leaf_edge_line_graph(sizes[si], extra_first != 0);
            for (size_t ai = 0; ai < 2; ++ai) {
                LineGraphResult r = check_line_graph(g, algos[ai]);
                ASSERT_TRUE(r.is_line_graph)
                    << "s=" << sizes[si] << " extra_first=" << extra_first << " algo=" << ai;
                EXPECT_TRUE(graph_recognition::gtest_utils::verify_krausz_partition(g, r))
                    << "s=" << sizes[si] << " extra_first=" << extra_first << " algo=" << ai;
            }
        }
    }
}

// The same construction with one more leaf edge (1,3) in H: vertex s+2 is
// adjacent to 1, 3 and (sharing the leaf 1) to s+1, so the common
// neighbourhood of (1,2) is K minus {1,2} plus the vertex s+1, and that of
// (1, s+1) is {2, s+2}, two isolated vertices.
TEST(LineGraphRegressionTest, LargeStarPlusTwoLeafEdgesIsALineGraph) {
    const int s = 33;
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= s; ++u)
        for (int v = u + 1; v <= s; ++v) edges.push_back(std::make_pair(u, v));
    edges.push_back(std::make_pair(1, s + 1));
    edges.push_back(std::make_pair(2, s + 1));
    edges.push_back(std::make_pair(1, s + 2));
    edges.push_back(std::make_pair(3, s + 2));
    edges.push_back(std::make_pair(s + 1, s + 2));
    Graph g(s + 2, edges);
    LineGraphResult r = check_line_graph(g);
    ASSERT_TRUE(r.is_line_graph);
    EXPECT_TRUE(graph_recognition::gtest_utils::verify_krausz_partition(g, r));

    // Without the edge (s+1, s+2), vertex 1 would need three cliques.
    edges.pop_back();
    Graph h(s + 2, edges);
    EXPECT_FALSE(check_line_graph(h).is_line_graph);
}

// A non-line graph whose first edge has a large non-clique common
// neighbourhood: K_{1,3} = claw glued into K_31 (vertex 32 adjacent to 1, 2
// and 3, which are pairwise adjacent, gives K_4 -- fine -- so instead attach
// three pendant vertices to 1: the claw on {1, 32, 33, 34}).
TEST(LineGraphRegressionTest, LargeCliqueWithAClawIsNotALineGraph) {
    const int s = 31;
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= s; ++u)
        for (int v = u + 1; v <= s; ++v) edges.push_back(std::make_pair(u, v));
    edges.push_back(std::make_pair(1, s + 1));
    edges.push_back(std::make_pair(1, s + 2));
    Graph g(s + 2, edges);
    // 1 lies in K_31 and in two K_2 cliques: three cliques -> not a line graph.
    EXPECT_FALSE(check_line_graph(g, graph_recognition::LineGraphAlgorithm::BRUTE).is_line_graph);
    EXPECT_FALSE(check_line_graph(g, graph_recognition::LineGraphAlgorithm::KRAUSZ).is_line_graph);
}

}  // namespace
