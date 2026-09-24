#include "util/polyhedral_edge_enum.h"
#include "enumerators/polyhedral_labeled_enum.h"
#include "enumerators/polyhedral_unlabeled_enum.h"
#include "recognizers/polyhedral.h"
#include "test_helpers.h"
#include <gtest/gtest.h>
#include <map>
#include <random>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using graph_recognition::gtest_utils::canonical_edge_list;
using Edges = std::vector<std::pair<int, int>>;

// Validate the embedding independently of the generator's face table.
void expect_polyhedral_embedding(const PlaneGraph& g) {
    const int n = static_cast<int>(g.rotation.size());
    std::set<std::pair<int, int>> darts;
    for (int u = 0; u < n; ++u) {
        ASSERT_GE(g.rotation[u].size(), 3u);
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
    int count = 0;
    while (!darts.empty()) {
        const auto start = *darts.begin();
        auto dart = start;
        std::set<int> boundary;
        do {
            ASSERT_EQ(darts.erase(dart), 1u);
            ASSERT_TRUE(boundary.insert(dart.first).second);
            const auto& row = g.rotation[dart.second];
            const auto pos = std::find(row.begin(), row.end(), dart.first) - row.begin();
            dart = {dart.second, row[(pos + 1) % row.size()]};
        } while (dart != start);
        ASSERT_GE(boundary.size(), 3u);
        ++count;
    }
    EXPECT_EQ(n - m + count, 2);
    EXPECT_TRUE(check_polyhedral(Graph(n, plane_graph_edges(g))).is_polyhedral);
}

TEST(PolyhedralEdgeEnumTest, FaceConditionMatchesAllVertexPairCutsThroughEightVertices) {
    std::mt19937 random(20260925);
    int non_degree_rejections = 0, nontriangular_deletions = 0;
    const std::size_t counts[] = {1, 2, 7, 34, 257};
    for (int n = 4; n <= 8; ++n) {
        // Explore every valid deletion, including edges with no triangular
        // incident face, with a global set instead of the production parent
        // rule. This independently exercises every local deletion condition.
        std::vector<PlaneGraphCanon> queue;
        std::set<std::vector<int>> seen;
        auto root = [&](const PlaneTriangulationCanon& g) {
            if (seen.insert(g.code).second) queue.push_back(g);
        };
        maximal_planar_embedded_expansion_dfs(
            plane_triangulation_canonicalize(plane_triangulation_tetrahedron()), n, root);
        for (std::size_t i = 0; i < queue.size(); ++i) {
            const auto current = queue[i];  // queue grows below
            const auto& g = current.graph;
            expect_polyhedral_embedding(g);
            const auto edges = plane_graph_edges(g);
            const auto faces = polyhedral_faces(g);
            EXPECT_EQ(faces.boundary.size(), edges.size() - n + 2);
            for (const auto& edge : edges) {
                const int u = edge.first - 1, v = edge.second - 1;
                const auto child = polyhedral_delete_edge(g, u, v);
                const bool expected = check_triconnected(Graph(n, plane_graph_edges(child))).is_triconnected;
                const bool actual = polyhedral_can_delete_edge(g, faces, u, v);
                ASSERT_EQ(actual, expected) << n << " " << u << " " << v;
                if (!actual) {
                    if (g.rotation[u].size() >= 4 && g.rotation[v].size() >= 4)
                        ++non_degree_rejections;
                    continue;
                }
                if (faces.boundary[faces.left[u][v]].size() > 3 &&
                    faces.boundary[faces.left[v][u]].size() > 3) ++nontriangular_deletions;
                const auto canonical = plane_graph_canonicalize(child);
                if (seen.insert(canonical.code).second) queue.push_back(canonical);
            }

            // Vary labels, cyclic starting positions and mirror orientation.
            std::vector<int> labels(n);
            for (int v = 0; v < n; ++v) labels[v] = v;
            std::shuffle(labels.begin(), labels.end(), random);
            PlaneGraph permuted;
            permuted.rotation.resize(n);
            for (int u = 0; u < n; ++u) {
                auto& row = permuted.rotation[labels[u]];
                for (int v : g.rotation[u]) row.push_back(labels[v]);
                std::rotate(row.begin(), row.begin() + random() % row.size(), row.end());
                if (i % 2) std::reverse(row.begin(), row.end());
            }
            EXPECT_EQ(plane_graph_canonicalize(permuted).code, current.code);
            if (edges.size() < static_cast<std::size_t>(3 * n - 6)) {
                const auto parent = polyhedral_edge_parent(g);
                expect_polyhedral_embedding(parent);
                const auto parent_edges = plane_graph_edges(parent);
                ASSERT_EQ(parent_edges.size(), edges.size() + 1);
                EXPECT_TRUE(std::includes(parent_edges.begin(), parent_edges.end(), edges.begin(), edges.end()));
            }
        }
        EXPECT_EQ(queue.size(), counts[n - 4]);
    }
    EXPECT_GT(non_degree_rejections, 0);
    EXPECT_GT(nontriangular_deletions, 0);
    EXPECT_THROW(polyhedral_edge_parent(plane_triangulation_tetrahedron()), std::logic_error);
}

TEST(PolyhedralEdgeEnumTest, DedicatedAndLegacyOutputSetsAgree) {
    using L = PolyhedralLabeledEnumAlgorithm;
    using U = PolyhedralUnlabeledEnumAlgorithm;
    for (int n = 0; n <= 7; ++n) {
        const auto dedicated = enumerate_polyhedral_unlabeled_graphs(n, U::PLANTRI);
        const auto defaults = enumerate_polyhedral_unlabeled_graphs(n);
        const auto legacy = enumerate_polyhedral_unlabeled_graphs(n, U::CANONICAL_AUGMENTATION);
        std::set<Edges> actual, expected;
        ASSERT_EQ(defaults.graphs.size(), dedicated.graphs.size());
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            EXPECT_EQ(defaults.graphs[i].edges, dedicated.graphs[i].edges);
            EXPECT_TRUE(actual.insert(canonical_edge_list(n, dedicated.graphs[i].edges)).second);
        }
        for (const auto& g : legacy.graphs)
            EXPECT_TRUE(expected.insert(canonical_edge_list(n, g.edges)).second);
        EXPECT_EQ(actual, expected) << n;

        if (n > 6) continue;
        const auto labeled = enumerate_polyhedral_labeled_graphs(n, L::PLANTRI);
        const auto labeled_default = enumerate_polyhedral_labeled_graphs(n);
        const auto labeled_legacy = enumerate_polyhedral_labeled_graphs(n, L::REVERSE_SEARCH);
        actual.clear();
        expected.clear();
        ASSERT_EQ(labeled_default.graphs.size(), labeled.graphs.size());
        for (std::size_t i = 0; i < labeled.graphs.size(); ++i) {
            EXPECT_EQ(labeled_default.graphs[i].edges, labeled.graphs[i].edges);
            EXPECT_TRUE(actual.insert(labeled.graphs[i].edges).second);
        }
        for (const auto& g : labeled_legacy.graphs) EXPECT_TRUE(expected.insert(g.edges).second);
        EXPECT_EQ(actual, expected) << n;
    }
    for (auto algorithm : {L::PLANTRI, L::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_polyhedral_labeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_polyhedral_labeled_graphs(65, algorithm), std::invalid_argument);
    }
    for (auto algorithm : {U::PLANTRI, U::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_polyhedral_unlabeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_polyhedral_unlabeled_graphs(64, algorithm), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_polyhedral_labeled_graphs(0, static_cast<L>(99)), std::invalid_argument);
    EXPECT_THROW(enumerate_polyhedral_unlabeled_graphs(0, static_cast<U>(99)), std::invalid_argument);
}

TEST(PolyhedralEdgeEnumTest, PublishedCountsAndSparseGraphs) {
    // Brinkmann--McKay 2007, tables of 3-connected simple plane graphs.
    const std::size_t counts[] = {2606, 32300};
    for (int n = 9; n <= 10; ++n) {
        const auto result = enumerate_polyhedral_unlabeled_graphs(n);
        EXPECT_EQ(result.graphs.size(), counts[n - 9]);
        std::size_t smallest = 3 * n;
        for (const auto& g : result.graphs) smallest = std::min(smallest, g.edges.size());
        EXPECT_EQ(smallest, static_cast<std::size_t>((3 * n + 1) / 2));
    }
    const auto six = enumerate_polyhedral_unlabeled_graphs(6);
    std::set<Edges> classes;
    for (const auto& g : six.graphs) classes.insert(canonical_edge_list(6, g.edges));
    // The triangular prism is not a triangulation; its missing diagonals
    // can lead to multiple triangulation roots, but it must appear once.
    const Edges prism = {{1,2}, {2,3}, {1,3}, {4,5}, {5,6}, {4,6}, {1,4}, {2,5}, {3,6}};
    EXPECT_EQ(classes.count(canonical_edge_list(6, prism)), 1u);
}
}  // namespace
