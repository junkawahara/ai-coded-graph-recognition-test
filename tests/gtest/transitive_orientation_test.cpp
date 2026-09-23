#include "decompositions/transitive_orientation.h"

#include "certificates.h"
#include "recognizers/comparability.h"
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

// review_all_1.md 11: a ragged orientation matrix is rejected by shape before
// any entry (in particular dir[v][u] of a not yet inspected row) is read.
TEST(TransitiveOrientationTest, VerifierRejectsRaggedMatrixBeforeReadingIt) {
    std::vector<std::vector<unsigned char>> a(3, std::vector<unsigned char>(3, 0));
    a[1][2] = a[2][1] = 1;

    std::vector<std::vector<int>> ragged(3);
    ragged[0].assign(3, 0);
    int row1[] = {0, 0, 1};
    ragged[1].assign(row1, row1 + 3);
    // ragged[2] stays empty
    EXPECT_FALSE(detail::orientation_is_transitive(a, ragged));

    std::vector<std::vector<int>> short_row0(3, std::vector<int>(3, 0));
    short_row0[0].clear();
    short_row0[1][2] = 1;
    short_row0[2][1] = -1;
    EXPECT_FALSE(detail::orientation_is_transitive(a, short_row0));

    std::vector<std::vector<int>> good(3, std::vector<int>(3, 0));
    good[1][2] = 1;
    good[2][1] = -1;
    EXPECT_TRUE(detail::orientation_is_transitive(a, good));
}
