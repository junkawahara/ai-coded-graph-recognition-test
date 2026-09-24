#include "util/plane_triangulation.h"
#include "recognizers/maximal_planar.h"
#include <gtest/gtest.h>
#include <map>
#include <random>
#include <set>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;

// Check a sphere embedding independently: symmetric simple adjacency,
// triangular face walks partitioning all darts, and Euler's formula.
void expect_triangulation(const PlaneTriangulation& g) {
    const int n = static_cast<int>(g.rotation.size());
    std::set<std::pair<int, int> > darts;
    for (int v = 0; v < n; ++v) {
        ASSERT_GE(g.rotation[v].size(), 3u);
        for (int w : g.rotation[v]) {
            ASSERT_GE(w, 0);
            ASSERT_LT(w, n);
            ASSERT_NE(w, v);
            ASSERT_TRUE(darts.insert({v, w}).second);
            ASSERT_NE(std::find(g.rotation[w].begin(), g.rotation[w].end(), v),
                      g.rotation[w].end());
        }
    }
    EXPECT_EQ(darts.size(), static_cast<std::size_t>(6 * n - 12));
    int faces = 0;
    while (!darts.empty()) {
        const auto start = *darts.begin();
        auto dart = start;
        for (int step = 0; step < 3; ++step) {
            ASSERT_EQ(darts.erase(dart), 1u);
            const auto& row = g.rotation[dart.second];
            const std::size_t index = std::find(row.begin(), row.end(), dart.first) - row.begin();
            dart = {dart.second, row[(index + 1) % row.size()]};
        }
        ASSERT_EQ(dart, start);
        ++faces;
    }
    EXPECT_EQ(faces, 2 * n - 4);
    EXPECT_TRUE(check_maximal_planar(Graph(n, plane_triangulation_edges(g))).is_maximal_planar);
}

TEST(PlaneTriangulationTest, LocalExpansionsPreserveFacesAndReduceToTheirParent) {
    std::vector<PlaneTriangulation> level(1, plane_triangulation_tetrahedron());
    for (int n = 4; n <= 7; ++n) {
        std::map<std::vector<int>, PlaneTriangulation> next;
        for (const auto& parent : level) {
            for (int degree = 3; degree <= 5; ++degree) {
                for (int v = 0; v < n; ++v) {
                    if (parent.rotation[v].size() < static_cast<std::size_t>(degree - 1)) continue;
                    for (int start = 0; start < static_cast<int>(parent.rotation[v].size()); ++start) {
                        const auto child = plane_triangulation_expand(parent, v, start, degree);
                        expect_triangulation(child);
                        PlaneTriangulation reduced;
                        ASSERT_TRUE(plane_triangulation_reduce(child, n, 0, reduced));
                        EXPECT_EQ(plane_triangulation_edges(reduced), plane_triangulation_edges(parent));
                        const auto canon = plane_triangulation_canonicalize(child);
                        next.emplace(canon.code, canon.graph);
                    }
                }
            }
        }
        level.clear();
        for (const auto& entry : next) level.push_back(entry.second);
    }
    EXPECT_EQ(level.size(), 14u);
}

TEST(PlaneTriangulationTest, CanonAndParentIgnoreLabelsCyclicStartsAndReflection) {
    std::mt19937 random(20260925);
    PlaneTriangulation g = plane_triangulation_tetrahedron();
    for (int n = 4; n < 20; ++n) {
        int v = random() % n;
        int degree = 3 + random() % 3;
        if (g.rotation[v].size() < static_cast<std::size_t>(degree - 1)) degree = 3;
        g = plane_triangulation_expand(g, v, random() % g.rotation[v].size(), degree);
        expect_triangulation(g);
        const auto expected = plane_triangulation_canonicalize(g);
        std::vector<int> labels(n + 1);
        for (int i = 0; i <= n; ++i) labels[i] = i;
        for (int trial = 0; trial < 4; ++trial) {
            std::shuffle(labels.begin(), labels.end(), random);
            PlaneTriangulation permuted;
            permuted.rotation.resize(n + 1);
            for (int u = 0; u <= n; ++u) {
                auto& row = permuted.rotation[labels[u]];
                for (int w : g.rotation[u]) row.push_back(labels[w]);
                std::rotate(row.begin(), row.begin() + random() % row.size(), row.end());
                if (trial % 2) std::reverse(row.begin(), row.end());
            }
            const auto actual = plane_triangulation_canonicalize(permuted);
            EXPECT_EQ(actual.code, expected.code);
            EXPECT_EQ(actual.graph.rotation, expected.graph.rotation);
            EXPECT_EQ(plane_triangulation_parent_code(actual.graph),
                      plane_triangulation_parent_code(expected.graph));
        }
    }
}

TEST(PlaneTriangulationTest, ReductionsRejectParallelDiagonalsAndTheRoot) {
    PlaneTriangulation parent;
    const auto tetrahedron = plane_triangulation_tetrahedron();
    EXPECT_FALSE(plane_triangulation_reduce(tetrahedron, 0, 0, parent));
    const auto five = plane_triangulation_expand(tetrahedron, 0, 0, 3);
    // A degree-four vertex in K5 minus an edge has one existing diagonal
    // and one missing diagonal. Only the latter is a legal reduction.
    int accepted = 0, rejected = 0;
    for (int v = 0; v < 5; ++v) {
        if (five.rotation[v].size() != 4) continue;
        for (int i = 0; i < 4; ++i) {
            if (plane_triangulation_reduce(five, v, i, parent)) {
                ++accepted;
                expect_triangulation(parent);
            } else ++rejected;
        }
    }
    EXPECT_EQ(accepted, 6);
    EXPECT_EQ(rejected, 6);
}
}  // namespace
