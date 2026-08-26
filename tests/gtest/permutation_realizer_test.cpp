#include "recognizers/permutation.h"

#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

// Rebuild the graph from the diagram: two vertices are adjacent exactly when
// their order differs between the two lines.
bool diagram_describes(const Graph& g, const PermutationRealizerResult& r) {
    int n = g.n;
    if ((int)r.pos1.size() != n + 1 || (int)r.pos2.size() != n + 1) return false;
    if ((int)r.pi.size() != n + 1) return false;

    std::vector<int> seen1(n + 2, 0), seen2(n + 2, 0);
    for (int v = 1; v <= n; ++v) {
        int p1 = r.pos1[v], p2 = r.pos2[v];
        if (p1 < 1 || p1 > n || seen1[p1]) return false;
        if (p2 < 1 || p2 > n || seen2[p2]) return false;
        seen1[p1] = 1;
        seen2[p2] = 1;
        if (r.pi[p1] != p2) return false;
    }
    for (int u = 1; u <= n; ++u) {
        for (int v = u + 1; v <= n; ++v) {
            bool crossing = (r.pos1[u] < r.pos1[v]) != (r.pos2[u] < r.pos2[v]);
            if (crossing != g.has_edge(u, v)) return false;
        }
    }
    return true;
}

TEST(PermutationRealizerTest, EmptyGraph) {
    Graph g;
    PermutationRealizerResult r = build_permutation_realizer(g);
    EXPECT_TRUE(r.is_permutation);
    EXPECT_TRUE(diagram_describes(g, r));
}

TEST(PermutationRealizerTest, SingleEdgeCrosses) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    Graph g(2, edges);
    PermutationRealizerResult r = build_permutation_realizer(g);
    ASSERT_TRUE(r.is_permutation);
    EXPECT_TRUE(diagram_describes(g, r));
    // The only crossing diagram on two vertices reverses them.
    EXPECT_NE(r.pos1[1] < r.pos1[2], r.pos2[1] < r.pos2[2]);
}

TEST(PermutationRealizerTest, PathOnFourVertices) {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    Graph g(4, edges);
    PermutationRealizerResult r = build_permutation_realizer(g);
    ASSERT_TRUE(r.is_permutation);
    EXPECT_TRUE(diagram_describes(g, r));
}

TEST(PermutationRealizerTest, CycleOnFiveVerticesIsNotAPermutationGraph) {
    std::vector<std::pair<int, int>> edges;
    for (int v = 1; v < 5; ++v) edges.push_back(std::make_pair(v, v + 1));
    edges.push_back(std::make_pair(5, 1));
    Graph g(5, edges);
    PermutationRealizerResult r = build_permutation_realizer(g);
    EXPECT_FALSE(r.is_permutation);
    EXPECT_TRUE(r.pos1.empty());
}

// Every graph on up to 6 vertices: a diagram is produced exactly when the
// recognizer says permutation graph, and it always describes the graph.
TEST(PermutationRealizerTest, ExhaustiveAgreementWithRecognizer) {
    const PermutationAlgorithm algos[] = {PermutationAlgorithm::CLASS_BASED,
                                          PermutationAlgorithm::BACKTRACKING};
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

            bool expect = check_permutation(g).is_permutation;
            for (size_t ai = 0; ai < 2; ++ai) {
                PermutationRealizerResult r = build_permutation_realizer(g, algos[ai]);
                ASSERT_EQ(r.is_permutation, expect)
                    << "n=" << n << " mask=" << mask << " algo=" << ai;
                if (!expect) continue;
                ASSERT_TRUE(diagram_describes(g, r))
                    << "n=" << n << " mask=" << mask << " algo=" << ai;
            }
        }
    }
}

}  // namespace
