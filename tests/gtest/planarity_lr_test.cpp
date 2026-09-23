#include "util/planarity_lr.h"

#include <gtest/gtest.h>

#include <utility>
#include <vector>

using namespace graph_recognition;
using detail_planar_lr::LRPlanarity;

namespace {

Graph make_complete_bipartite(int a, int b) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= a; ++u)
        for (int v = a + 1; v <= a + b; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(a + b, edges);
}

Graph make_complete(int n) {
    std::vector<std::pair<int, int>> edges;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) edges.push_back(std::make_pair(u, v));
    return Graph(n, edges);
}

Graph make_cube() {
    int e[12][2] = {{1, 2}, {2, 3}, {3, 4}, {4, 1}, {5, 6}, {6, 7},
                    {7, 8}, {8, 5}, {1, 5}, {2, 6}, {3, 7}, {4, 8}};
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < 12; ++i) edges.push_back(std::make_pair(e[i][0], e[i][1]));
    return Graph(8, edges);
}

// review_all_1.md 9: run() used to reuse the working state of the previous
// call, so a second run on K_{3,3} answered "planar".
TEST(LRPlanarityTest, RunIsRepeatableOnTheSameInstance) {
    LRPlanarity k33(make_complete_bipartite(3, 3));
    EXPECT_FALSE(k33.run());
    EXPECT_FALSE(k33.run());
    EXPECT_FALSE(k33.run());

    LRPlanarity k5(make_complete(5));
    EXPECT_FALSE(k5.run());
    EXPECT_FALSE(k5.run());

    LRPlanarity cube(make_cube());
    EXPECT_TRUE(cube.run());
    EXPECT_TRUE(cube.run());

    LRPlanarity k4(make_complete(4));
    EXPECT_TRUE(k4.run());
    EXPECT_TRUE(k4.run());
}

TEST(LRPlanarityTest, FreeFunctionAgreesWithClass) {
    Graph gs[] = {make_complete_bipartite(3, 3), make_complete(5), make_cube(), make_complete(4),
                  make_complete_bipartite(2, 4)};
    for (size_t i = 0; i < sizeof(gs) / sizeof(gs[0]); ++i) {
        LRPlanarity lr(gs[i]);
        EXPECT_EQ(detail_planar_lr::is_planar_lr(gs[i]), lr.run()) << "graph " << i;
    }
}

}  // namespace
