#include "recognizers/distance_hereditary.h"

#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

// Independent replay: each step must be legal in the graph left at that
// point, and exactly one vertex must survive.
bool replay(const Graph& g, const std::vector<PruningStep>& steps) {
    int n = g.n;
    if (n == 0) return steps.empty();
    if ((int)steps.size() != n - 1) return false;

    std::vector<bool> alive(n + 1, true);
    for (size_t i = 0; i < steps.size(); ++i) {
        int v = steps[i].vertex, w = steps[i].witness, kind = steps[i].kind;
        if (v < 1 || v > n || !alive[v]) return false;

        std::vector<int> nbrs;
        for (int u = 1; u <= n; ++u) {
            if (alive[u] && u != v && g.has_edge(v, u)) nbrs.push_back(u);
        }
        if (kind == 0) {
            if (!nbrs.empty() || w != 0) return false;
        } else if (kind == 1) {
            if (nbrs.size() != 1 || nbrs[0] != w) return false;
        } else if (kind == 2 || kind == 3) {
            if (w < 1 || w > n || !alive[w] || w == v) return false;
            if (g.has_edge(v, w) != (kind == 2)) return false;
            for (int u = 1; u <= n; ++u) {
                if (!alive[u] || u == v || u == w) continue;
                if (g.has_edge(v, u) != g.has_edge(w, u)) return false;
            }
        } else {
            return false;
        }
        alive[v] = false;
    }

    int left = 0;
    for (int v = 1; v <= n; ++v) {
        if (alive[v]) ++left;
    }
    return left == 1;
}

TEST(PruningSequenceTest, EmptyGraphHasNoSteps) {
    Graph g;
    PruningSequenceResult r = build_pruning_sequence(g);
    EXPECT_TRUE(r.is_distance_hereditary);
    EXPECT_TRUE(r.steps.empty());
}

TEST(PruningSequenceTest, PathPrunesFromTheEnds) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    Graph g(5, edges);
    PruningSequenceResult r = build_pruning_sequence(g);
    ASSERT_TRUE(r.is_distance_hereditary);
    EXPECT_EQ(r.steps.size(), 4u);
    EXPECT_TRUE(replay(g, r.steps));
}

TEST(PruningSequenceTest, CompleteGraphPrunesAsTrueTwins) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= 4; ++u)
        for (int v = u + 1; v <= 4; ++v) edges.push_back(std::make_pair(u, v));
    Graph g(4, edges);
    PruningSequenceResult r = build_pruning_sequence(g);
    ASSERT_TRUE(r.is_distance_hereditary);
    ASSERT_EQ(r.steps.size(), 3u);
    // While three or more vertices remain they are mutual true twins; the last
    // step happens with two left, where each is a pendant of the other and the
    // cheaper pendant rule fires first.
    EXPECT_EQ(r.steps[0].kind, 2);
    EXPECT_EQ(r.steps[1].kind, 2);
    EXPECT_EQ(r.steps[2].kind, 1);
    EXPECT_TRUE(replay(g, r.steps));
}

TEST(PruningSequenceTest, EdgelessGraphPrunesIsolatedVertices) {
    Graph g(3, std::vector<std::pair<int, int>>());
    PruningSequenceResult r = build_pruning_sequence(g);
    ASSERT_TRUE(r.is_distance_hereditary);
    ASSERT_EQ(r.steps.size(), 2u);
    for (size_t i = 0; i < r.steps.size(); ++i) EXPECT_EQ(r.steps[i].kind, 0);
    EXPECT_TRUE(replay(g, r.steps));
}

TEST(PruningSequenceTest, CycleOnFiveVerticesIsNotDistanceHereditary) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(5, 1));
    Graph g(5, edges);
    PruningSequenceResult r = build_pruning_sequence(g);
    EXPECT_FALSE(r.is_distance_hereditary);
    EXPECT_TRUE(r.steps.empty());
}

// Every graph on up to 6 vertices: a sequence is produced exactly when the
// recognizers say distance-hereditary, and it always replays.
TEST(PruningSequenceTest, ExhaustiveAgreementWithRecognizer) {
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

            bool expect = check_distance_hereditary(g).is_distance_hereditary;
            PruningSequenceResult r = build_pruning_sequence(g);
            ASSERT_EQ(r.is_distance_hereditary, expect) << "n=" << n << " mask=" << mask;
            if (!expect) continue;
            ASSERT_TRUE(replay(g, r.steps)) << "n=" << n << " mask=" << mask;
        }
    }
}

}  // namespace
