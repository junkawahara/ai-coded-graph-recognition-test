#include "enumerators/planar_labeled_enum.h"
#include "enumerators/planar_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>
#include <map>
#include <random>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using gtest_utils::canonical_edge_list;
using Edges = std::vector<std::pair<int, int>>;

std::vector<unsigned long long> adjacency(int n, const Edges& edges) {
    std::vector<unsigned long long> adj(n, 0);
    for (const auto& e : edges) {
        adj[e.first - 1] |= 1ULL << (e.second - 1);
        adj[e.second - 1] |= 1ULL << (e.first - 1);
    }
    return adj;
}

// Independent dart traversal: facial boundaries may repeat vertices, but
// each dart appears once, and Euler's formula must describe a sphere.
void expect_embedding(const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    std::set<std::pair<int, int>> darts;
    for (int u = 0; u < n; ++u) {
        for (int v : g.rotation[u]) {
            ASSERT_GE(v, 0);
            ASSERT_LT(v, n);
            ASSERT_NE(u, v);
            ASSERT_TRUE(darts.insert({u, v}).second);
            ASSERT_NE(std::find(g.rotation[v].begin(), g.rotation[v].end(), u),
                      g.rotation[v].end());
        }
    }
    const int m = static_cast<int>(darts.size()) / 2;
    int faces = m == 0 ? 1 : 0;
    while (!darts.empty()) {
        const auto start = *darts.begin();
        auto dart = start;
        do {
            ASSERT_EQ(darts.erase(dart), 1u);
            const auto& row = g.rotation[dart.second];
            const auto pos = std::find(row.begin(), row.end(), dart.first) - row.begin();
            dart = {dart.second, row[(pos + 1) % row.size()]};
        } while (dart != start);
        ++faces;
    }
    EXPECT_EQ(n - m + faces, 2);
    const auto edges = plane_graph_edges(g);
    EXPECT_TRUE(bitmask_graph_connected(n, adjacency(n, edges)));
    EXPECT_TRUE(check_planar(Graph(n, edges)).is_planar);
}

TEST(PlanarEdgeEnumTest, CanonicalSearchMatchesUnrestrictedConnectedDeletions) {
    std::mt19937 random(20260925);
    int bridges = 0, nontriangular_deletions = 0, repeated_boundaries = 0;
    for (int n = 3; n <= 7; ++n) {
        // Independent closure under ALL connectivity-preserving deletions,
        // using a visited set rather than a canonical parent or facial test.
        std::vector<PlaneGraphCanon> queue;
        std::set<std::vector<int>> all_maps, actual;
        auto root = [&](const PlaneGraphCanon& map) {
            if (all_maps.insert(map.code).second) queue.push_back(map);
        };
        if (n == 3) {
            PlaneGraph triangle;
            triangle.rotation = {{1, 2}, {0, 2}, {0, 1}};
            root(plane_graph_canonicalize(triangle));
        } else {
            maximal_planar_embedded_expansion_dfs(
                plane_triangulation_canonicalize(plane_triangulation_tetrahedron()), n, root);
        }
        for (std::size_t i = 0; i < queue.size(); ++i) {
            const auto current = queue[i];
            const auto edges = plane_graph_edges(current.graph);
            expect_embedding(current.graph);
            const auto faces = plane_graph_faces(current.graph);
            for (const auto& face : faces.boundary)
                if (std::set<int>(face.begin(), face.end()).size() < face.size()) ++repeated_boundaries;
            for (const auto& edge : edges) {
                const int u = edge.first - 1, v = edge.second - 1;
                const auto child = plane_graph_delete_edge(current.graph, u, v);
                const bool connected = bitmask_graph_connected(n, adjacency(n, plane_graph_edges(child)));
                const int a = faces.left[u][v], b = faces.left[v][u];
                ASSERT_EQ(connected, a != b);
                if (!connected) { ++bridges; continue; }
                if (faces.boundary[a].size() != 3 && faces.boundary[b].size() != 3)
                    ++nontriangular_deletions;
                root(plane_graph_canonicalize(child));
            }
            if (edges.size() < static_cast<std::size_t>(3 * n - 6)) {
                const auto parent = plane_graph_edge_parent(current.graph);
                expect_embedding(parent);
                const auto parent_edges = plane_graph_edges(parent);
                EXPECT_EQ(parent_edges.size(), edges.size() + 1);
                EXPECT_TRUE(std::includes(parent_edges.begin(), parent_edges.end(), edges.begin(), edges.end()));
            }
            // Labels, rotation starting positions and reflection are immaterial.
            std::vector<int> labels(n);
            for (int v = 0; v < n; ++v) labels[v] = v;
            std::shuffle(labels.begin(), labels.end(), random);
            PlaneGraph permuted;
            permuted.rotation.resize(n);
            for (int u = 0; u < n; ++u) {
                auto& row = permuted.rotation[labels[u]];
                for (int v : current.graph.rotation[u]) row.push_back(labels[v]);
                std::rotate(row.begin(), row.begin() + random() % row.size(), row.end());
                if (i % 2) std::reverse(row.begin(), row.end());
            }
            EXPECT_EQ(plane_graph_canonicalize(permuted).code, current.code);
        }
        planar_connected_embedded_enumerate(n, [&](const PlaneGraphCanon& map) {
            EXPECT_TRUE(actual.insert(map.code).second);
        });
        EXPECT_EQ(actual, all_maps) << n;
    }
    EXPECT_GT(bridges, 0);
    EXPECT_GT(nontriangular_deletions, 0);
    EXPECT_GT(repeated_boundaries, 0);
}

TEST(PlanarEdgeEnumTest, PublishedMapCountsDifferFromAbstractGraphCounts) {
    // Brinkmann--McKay 2007, Table 19, mirror images identified. The
    // eight-vertex distribution exercises every edge count down to trees.
    const std::size_t maps[] = {0, 1, 1, 2, 6, 25, 179, 2014, 31178};
    const std::size_t eight_by_edges[] = {27, 271, 1293, 3539, 6205, 7482,
                                        6318, 3833, 1623, 485, 88, 14};
    for (int n = 1; n <= 8; ++n) {
        std::map<std::size_t, std::size_t> by_edges;
        std::size_t count = 0;
        planar_connected_embedded_enumerate(n, [&](const PlaneGraphCanon& map) {
            ++count;
            ++by_edges[plane_graph_edges(map.graph).size()];
        });
        EXPECT_EQ(count, maps[n]) << n;
        if (n == 8) {
            for (int m = 7; m <= 18; ++m) {
                EXPECT_EQ(by_edges[m], eight_by_edges[m - 7]) << m;
            }
        }
    }
    EXPECT_EQ(enumerate_planar_unlabeled_graphs(8, true).graphs.size(), 5974u);
    EXPECT_EQ(enumerate_planar_unlabeled_graphs(8).graphs.size(), 6966u);

    // A triangle with two leaves at the same vertex: both leaves in one
    // face versus one in each face. Distinct maps, same abstract graph.
    PlaneGraph a, b;
    a.rotation = {{1, 2, 3, 4}, {0, 2}, {0, 1}, {0}, {0}};
    b.rotation = {{1, 3, 2, 4}, {0, 2}, {0, 1}, {0}, {0}};
    expect_embedding(a);
    expect_embedding(b);
    EXPECT_NE(plane_graph_canonicalize(a).code, plane_graph_canonicalize(b).code);
    EXPECT_EQ(plane_graph_edges(a), plane_graph_edges(b));
    std::size_t copies = 0;
    const auto target = canonical_edge_list(5, plane_graph_edges(a));
    for (const auto& g : enumerate_planar_unlabeled_graphs(5, true).graphs)
        if (canonical_edge_list(5, g.edges) == target) ++copies;
    EXPECT_EQ(copies, 1u);
}

TEST(PlanarEdgeEnumTest, BothLegacySetsAndDefaultsAgreeIncludingDisconnectedGraphs) {
    using L = PlanarLabeledEnumAlgorithm;
    using U = PlanarUnlabeledEnumAlgorithm;
    for (int n = 0; n <= 7; ++n) {
        for (bool connected : {false, true}) {
            const auto dedicated = enumerate_planar_unlabeled_graphs(n, connected, U::PLANTRI);
            const auto defaults = enumerate_planar_unlabeled_graphs(n, connected);
            const auto legacy = enumerate_planar_unlabeled_graphs(n, connected, U::CANONICAL_AUGMENTATION);
            std::set<Edges> actual, expected;
            ASSERT_EQ(defaults.graphs.size(), dedicated.graphs.size());
            for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
                EXPECT_EQ(defaults.graphs[i].edges, dedicated.graphs[i].edges);
                EXPECT_TRUE(actual.insert(canonical_edge_list(n, dedicated.graphs[i].edges)).second);
            }
            for (const auto& g : legacy.graphs) expected.insert(canonical_edge_list(n, g.edges));
            EXPECT_EQ(actual, expected) << n << " " << connected;
        }
        if (n > 6) continue;
        const auto dedicated = enumerate_planar_labeled_graphs_reverse_search(n, L::PLANTRI);
        const auto defaults = enumerate_planar_labeled_graphs_reverse_search(n);
        const auto legacy = enumerate_planar_labeled_graphs_reverse_search(n, L::REVERSE_SEARCH);
        std::set<Edges> actual, expected;
        ASSERT_EQ(defaults.graphs.size(), dedicated.graphs.size());
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            EXPECT_EQ(defaults.graphs[i].edges, dedicated.graphs[i].edges);
            EXPECT_TRUE(actual.insert(dedicated.graphs[i].edges).second);
        }
        for (const auto& g : legacy.graphs) expected.insert(g.edges);
        EXPECT_EQ(actual, expected) << n;
        if (n == 6) {
            // Repeated nontrivial components must appear exactly once per
            // labeling, and arbitrary isolated labels must also be reached.
            EXPECT_EQ(actual.count({{1,2}, {1,3}, {2,3}, {4,5}, {4,6}, {5,6}}), 1u);
            EXPECT_EQ(actual.count({{2,4}, {2,6}, {4,6}}), 1u);
        }
    }
    for (auto algorithm : {L::PLANTRI, L::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_planar_labeled_graphs_reverse_search(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_planar_labeled_graphs_reverse_search(65, algorithm), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_planar_labeled_graphs_reverse_search(64, L::PLANTRI), std::invalid_argument);
    for (auto algorithm : {U::PLANTRI, U::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_planar_unlabeled_graphs(-1, false, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_planar_unlabeled_graphs(64, false, algorithm), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_planar_labeled_graphs_reverse_search(0, static_cast<L>(99)), std::invalid_argument);
    EXPECT_THROW(enumerate_planar_unlabeled_graphs(0, false, static_cast<U>(99)), std::invalid_argument);
}
}  // namespace
