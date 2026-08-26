#include "decompositions/elimination_orderings.h"

#include "certificates.h"
#include "recognizers/strongly_chordal.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::verify_seo;

namespace {

Graph make_cycle(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < n; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(n, 1));
    return Graph(n, edges);
}

// The 3-sun: the smallest chordal graph that is not strongly chordal.
Graph make_sun3() {
    std::vector<std::pair<int, int>> edges;
    // Inner triangle 1-2-3, outer independent set 4, 5, 6.
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 1));
    edges.push_back(std::make_pair(4, 1));
    edges.push_back(std::make_pair(4, 2));
    edges.push_back(std::make_pair(5, 2));
    edges.push_back(std::make_pair(5, 3));
    edges.push_back(std::make_pair(6, 3));
    edges.push_back(std::make_pair(6, 1));
    return Graph(6, edges);
}

TEST(StrongEliminationTest, EmptyGraph) {
    Graph g;
    StrongEliminationResult r = compute_strong_elimination_ordering(g);
    EXPECT_TRUE(r.success);
    EXPECT_EQ(r.order.size(), 1u);
}

TEST(StrongEliminationTest, CompleteGraphOrdersEveryVertex) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 4; ++u)
        for (int v = u + 1; v <= 4; ++v) edges.push_back(std::make_pair(u, v));
    Graph g(4, edges);
    StrongEliminationResult r = compute_strong_elimination_ordering(g);
    ASSERT_TRUE(r.success);
    EXPECT_TRUE(verify_seo(g, r.order, r.number));
}

TEST(StrongEliminationTest, ThreeSunHasNoStrongEliminationOrdering) {
    Graph g = make_sun3();
    // Chordal, but not strongly chordal: the classic counterexample.
    EXPECT_FALSE(compute_strong_elimination_ordering(g).success);
    EXPECT_FALSE(check_strongly_chordal(g).is_strongly_chordal);
}

TEST(StrongEliminationTest, LongCycleIsNotEvenChordal) {
    Graph g = make_cycle(5);
    EXPECT_FALSE(compute_strong_elimination_ordering(g).success);
}

// Every graph on up to 6 vertices: Farber's construction must succeed exactly
// on the strongly chordal ones, and the ordering it builds must have the
// strong elimination property.
TEST(StrongEliminationTest, ExhaustiveAgreementWithRecognizer) {
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

            bool expect = check_strongly_chordal(g).is_strongly_chordal;
            StrongEliminationResult r = compute_strong_elimination_ordering(g);
            ASSERT_EQ(r.success, expect) << "n=" << n << " mask=" << mask;
            if (!expect) continue;
            ASSERT_TRUE(verify_seo(g, r.order, r.number)) << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
