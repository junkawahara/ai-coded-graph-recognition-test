#include "transitive_orientation.h"

#include "certificates.h"
#include "comparability.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::verify_transitive_orientation;

namespace {

Graph make_path(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    return Graph(n, edges);
}

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

TEST(TransitiveOrientationTest, EmptyAndEdgelessGraphs) {
    Graph empty;
    TransitiveOrientationResult r = transitive_orientation(empty);
    EXPECT_TRUE(r.is_comparability);
    EXPECT_TRUE(r.orientation.empty());

    Graph edgeless(4, std::vector<std::pair<int, int>>());
    TransitiveOrientationResult r2 = transitive_orientation(edgeless);
    EXPECT_TRUE(r2.is_comparability);
    EXPECT_TRUE(r2.orientation.empty());
    EXPECT_TRUE(verify_transitive_orientation(edgeless, r2));
}

TEST(TransitiveOrientationTest, PathIsAComparabilityGraph) {
    Graph g = make_path(4);
    TransitiveOrientationResult r = transitive_orientation(g);
    ASSERT_TRUE(r.is_comparability);
    EXPECT_EQ(r.orientation.size(), 3u);
    EXPECT_TRUE(verify_transitive_orientation(g, r));
}

TEST(TransitiveOrientationTest, OddCycleOfLengthFiveIsNot) {
    Graph g = make_cycle(5);
    EXPECT_FALSE(transitive_orientation(g).is_comparability);
    EXPECT_FALSE(
        transitive_orientation(g, TransitiveOrientationAlgorithm::BACKTRACKING).is_comparability);
}

TEST(TransitiveOrientationTest, EvenCycleOfLengthSixIsAComparabilityGraph) {
    Graph g = make_cycle(6);
    TransitiveOrientationResult r = transitive_orientation(g);
    ASSERT_TRUE(r.is_comparability);
    EXPECT_TRUE(verify_transitive_orientation(g, r));
}

TEST(TransitiveOrientationTest, MatrixEntryPointMatchesGraphEntryPoint) {
    Graph g = make_path(5);
    TransitiveOrientationResult a = transitive_orientation(g);
    TransitiveOrientationResult b = transitive_orientation_matrix(build_adj_matrix(g));
    EXPECT_EQ(a.is_comparability, b.is_comparability);
    EXPECT_EQ(a.orientation, b.orientation);
}

// Every graph on up to 6 vertices: both algorithms must agree with the
// recognizer, and every orientation they return must survive the independent
// transitivity check.
TEST(TransitiveOrientationTest, ExhaustiveAgreementWithRecognizer) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            bool expect = check_comparability(g).is_comparability;
            TransitiveOrientationResult forcing =
                transitive_orientation(g, TransitiveOrientationAlgorithm::FORCING);
            TransitiveOrientationResult backtracking =
                transitive_orientation(g, TransitiveOrientationAlgorithm::BACKTRACKING);

            ASSERT_EQ(forcing.is_comparability, expect) << "n=" << n << " mask=" << mask;
            ASSERT_EQ(backtracking.is_comparability, expect) << "n=" << n << " mask=" << mask;
            if (!expect) continue;
            ASSERT_TRUE(verify_transitive_orientation(g, forcing))
                << "n=" << n << " mask=" << mask;
            ASSERT_TRUE(verify_transitive_orientation(g, backtracking))
                << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
