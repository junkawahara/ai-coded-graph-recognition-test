#include "decompositions/components.h"
#include "util/graph_utils.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

Graph make_p4() {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 4));
    return Graph(4, edges);
}

// Two triangles plus an isolated vertex: 3 components, one of them trivial.
Graph make_two_triangles_plus_isolated() {
    std::vector<std::pair<int, int>> edges;
    edges.push_back(std::make_pair(1, 2));
    edges.push_back(std::make_pair(2, 3));
    edges.push_back(std::make_pair(3, 1));
    edges.push_back(std::make_pair(4, 5));
    edges.push_back(std::make_pair(5, 6));
    edges.push_back(std::make_pair(6, 4));
    return Graph(7, edges);
}

std::set<std::pair<int, int>> edge_set(const Graph& g) {
    std::set<std::pair<int, int>> s;
    for (int u = 1; u <= g.n; ++u) {
        for (size_t i = 0; i < g.adj[u].size(); ++i) {
            int v = g.adj[u][i];
            if (u < v) s.insert(std::make_pair(u, v));
        }
    }
    return s;
}

TEST(GraphUtilsTest, ComplementIsInvolutiveAndDisjoint) {
    Graph g = make_p4();
    Graph gc = build_complement(g);
    EXPECT_EQ(gc.n, g.n);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = u + 1; v <= g.n; ++v) {
            EXPECT_NE(g.has_edge(u, v), gc.has_edge(u, v));
        }
    }
    EXPECT_EQ(edge_set(build_complement(gc)), edge_set(g));
}

TEST(GraphUtilsTest, ComplementOfEmptyGraphIsEmpty) {
    Graph g;
    Graph gc = build_complement(g);
    EXPECT_EQ(gc.n, 0);
    EXPECT_TRUE(edge_set(gc).empty());
}

TEST(GraphUtilsTest, AdjMatrixMatchesGraph) {
    Graph g = make_p4();
    std::vector<std::vector<unsigned char>> a = build_adj_matrix(g);
    ASSERT_EQ(a.size(), (size_t)g.n + 1);
    for (int u = 1; u <= g.n; ++u) {
        EXPECT_EQ(a[u][u], 0);
        for (int v = 1; v <= g.n; ++v) {
            EXPECT_EQ(a[u][v] != 0, g.has_edge(u, v));
        }
    }

    std::vector<std::vector<unsigned char>> c = build_complement_matrix(a);
    for (int u = 1; u <= g.n; ++u) {
        EXPECT_EQ(c[u][u], 0);
        for (int v = 1; v <= g.n; ++v) {
            if (u == v) continue;
            EXPECT_EQ(c[u][v] != 0, !g.has_edge(u, v));
        }
    }
}

TEST(GraphUtilsTest, InducedSubgraphRenumbersInGivenOrder) {
    Graph g = make_p4();
    std::vector<int> verts;
    verts.push_back(4);
    verts.push_back(2);
    verts.push_back(3);
    std::vector<int> orig;
    Graph h = induced_subgraph(g, verts, &orig);

    ASSERT_EQ(h.n, 3);
    ASSERT_EQ(orig.size(), 4u);
    EXPECT_EQ(orig[1], 4);
    EXPECT_EQ(orig[2], 2);
    EXPECT_EQ(orig[3], 3);
    // Original edges among {2,3,4} are 2-3 and 3-4, i.e. local 2-3 and 1-3.
    EXPECT_TRUE(h.has_edge(2, 3));
    EXPECT_TRUE(h.has_edge(1, 3));
    EXPECT_FALSE(h.has_edge(1, 2));
}

TEST(GraphUtilsTest, InducedSubgraphDropsInvalidAndDuplicateVertices) {
    Graph g = make_p4();
    std::vector<int> verts;
    verts.push_back(2);
    verts.push_back(2);  // duplicate
    verts.push_back(0);  // out of range
    verts.push_back(9);  // out of range
    verts.push_back(3);
    Graph h = induced_subgraph(g, verts);
    EXPECT_EQ(h.n, 2);
    EXPECT_TRUE(h.has_edge(1, 2));
}

TEST(ComponentsTest, CountsAndLabelsComponents) {
    Graph g = make_two_triangles_plus_isolated();
    ComponentsResult cc = connected_components(g);
    EXPECT_EQ(cc.count, 3);
    ASSERT_EQ(cc.comp.size(), (size_t)g.n + 1);
    ASSERT_EQ(cc.vertices.size(), 3u);

    // Vertices in the same triangle share a label; the two triangles differ.
    EXPECT_EQ(cc.comp[1], cc.comp[2]);
    EXPECT_EQ(cc.comp[2], cc.comp[3]);
    EXPECT_EQ(cc.comp[4], cc.comp[5]);
    EXPECT_NE(cc.comp[1], cc.comp[4]);
    EXPECT_NE(cc.comp[1], cc.comp[7]);

    size_t total = 0;
    for (size_t i = 0; i < cc.vertices.size(); ++i) {
        total += cc.vertices[i].size();
        for (size_t j = 0; j < cc.vertices[i].size(); ++j) {
            EXPECT_EQ(cc.comp[cc.vertices[i][j]], (int)i + 1);
        }
    }
    EXPECT_EQ(total, (size_t)g.n);
}

TEST(ComponentsTest, EmptyGraphHasNoComponents) {
    Graph g;
    ComponentsResult cc = connected_components(g);
    EXPECT_EQ(cc.count, 0);
    EXPECT_EQ(cc.comp.size(), 1u);
    ComponentsResult co = co_components(g);
    EXPECT_EQ(co.count, 0);
}

TEST(ComponentsTest, CoComponentsMatchComplementComponents) {
    Graph g = make_two_triangles_plus_isolated();
    ComponentsResult co = co_components(g);
    ComponentsResult ref = connected_components(build_complement(g));
    ASSERT_EQ(co.count, ref.count);
    for (int u = 1; u <= g.n; ++u) {
        for (int v = 1; v <= g.n; ++v) {
            EXPECT_EQ(co.comp[u] == co.comp[v], ref.comp[u] == ref.comp[v]);
        }
    }
}

TEST(ComponentsTest, InducedVariantsUseOriginalVertexNumbers) {
    Graph g = make_two_triangles_plus_isolated();
    std::vector<int> verts;
    verts.push_back(1);
    verts.push_back(2);
    verts.push_back(4);
    std::vector<std::vector<int>> comps = induced_components(g, verts);
    ASSERT_EQ(comps.size(), 2u);
    for (size_t i = 0; i < comps.size(); ++i) std::sort(comps[i].begin(), comps[i].end());
    std::sort(comps.begin(), comps.end());
    ASSERT_EQ(comps[0].size(), 2u);
    EXPECT_EQ(comps[0][0], 1);
    EXPECT_EQ(comps[0][1], 2);
    ASSERT_EQ(comps[1].size(), 1u);
    EXPECT_EQ(comps[1][0], 4);

    // In the complement of g[{1,2,4}] the edge 1-2 disappears and 4 becomes
    // adjacent to both, so the co-components collapse to a single set.
    std::vector<std::vector<int>> cocomps = induced_co_components(g, verts);
    ASSERT_EQ(cocomps.size(), 1u);
    EXPECT_EQ(cocomps[0].size(), 3u);
}

TEST(ComponentsTest, InducedVariantsIgnoreInvalidAndDuplicateVertices) {
    Graph g = make_p4();
    std::vector<int> verts;
    verts.push_back(1);
    verts.push_back(1);
    verts.push_back(0);
    verts.push_back(99);
    std::vector<std::vector<int>> comps = induced_components(g, verts);
    ASSERT_EQ(comps.size(), 1u);
    EXPECT_EQ(comps[0].size(), 1u);

    std::vector<std::vector<int>> cocomps = induced_co_components(g, verts);
    ASSERT_EQ(cocomps.size(), 1u);
    EXPECT_EQ(cocomps[0].size(), 1u);
}

// Exhaustive cross-check of both component routines against a straightforward
// reachability oracle, over every graph on up to 5 vertices.
TEST(ComponentsTest, ExhaustiveAgreementWithBruteForce) {
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        }
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i) {
                if (mask & (1 << i)) edges.push_back(all[i]);
            }
            Graph g(n, edges);

            // Oracle: transitive closure of adjacency.
            std::vector<std::vector<char>> reach(n + 1, std::vector<char>(n + 1, 0));
            for (int v = 1; v <= n; ++v) reach[v][v] = 1;
            for (int u = 1; u <= n; ++u) {
                for (int v = 1; v <= n; ++v) {
                    if (g.has_edge(u, v)) reach[u][v] = 1;
                }
            }
            for (int k = 1; k <= n; ++k) {
                for (int u = 1; u <= n; ++u) {
                    for (int v = 1; v <= n; ++v) {
                        if (reach[u][k] && reach[k][v]) reach[u][v] = 1;
                    }
                }
            }

            ComponentsResult cc = connected_components(g);
            for (int u = 1; u <= n; ++u) {
                for (int v = 1; v <= n; ++v) {
                    EXPECT_EQ(cc.comp[u] == cc.comp[v], reach[u][v] != 0)
                        << "n=" << n << " mask=" << mask << " u=" << u << " v=" << v;
                }
            }

            ComponentsResult co = co_components(g);
            ComponentsResult ref = connected_components(build_complement(g));
            for (int u = 1; u <= n; ++u) {
                for (int v = 1; v <= n; ++v) {
                    EXPECT_EQ(co.comp[u] == co.comp[v], ref.comp[u] == ref.comp[v])
                        << "n=" << n << " mask=" << mask;
                }
            }
        }
    }
}

}  // namespace
