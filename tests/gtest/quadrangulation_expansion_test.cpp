#include "enumerators/simple_quadrangulation_unlabeled_enum.h"
#include "recognizers/simple_quadrangulation.h"
#include "util/canonical_augmentation.h"
#include "test_helpers.h"
#include <gtest/gtest.h>
#include <climits>
#include <map>
#include <random>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using Algorithm = SimpleQuadrangulationUnlabeledEnumAlgorithm;
using Edges = std::vector<std::pair<int, int>>;

// Independent dart traversal, rather than trusting the generator's face
// table: every dart occurs once in a 4-cycle, and Euler gives a sphere.
void expect_quadrangulation(const PlaneGraph& g, bool polyhedral = true) {
    const int n = static_cast<int>(g.rotation.size());
    std::set<std::pair<int, int>> darts;
    for (int v = 0; v < n; ++v) {
        ASSERT_GE(g.rotation[v].size(), 3u);
        for (int w : g.rotation[v]) {
            ASSERT_GE(w, 0);
            ASSERT_LT(w, n);
            ASSERT_NE(v, w);
            ASSERT_TRUE(darts.insert({v, w}).second);
            ASSERT_NE(std::find(g.rotation[w].begin(), g.rotation[w].end(), v),
                      g.rotation[w].end());
        }
    }
    EXPECT_EQ(darts.size(), static_cast<std::size_t>(4 * n - 8));
    int faces = 0;
    while (!darts.empty()) {
        const auto start = *darts.begin();
        auto dart = start;
        std::set<int> vertices;
        for (int step = 0; step < 4; ++step) {
            ASSERT_EQ(darts.erase(dart), 1u);
            EXPECT_TRUE(vertices.insert(dart.first).second);
            const auto& row = g.rotation[dart.second];
            const auto pos = std::find(row.begin(), row.end(), dart.first) - row.begin();
            dart = {dart.second, row[(pos + 1) % row.size()]};
        }
        ASSERT_EQ(dart, start);
        ++faces;
    }
    EXPECT_EQ(faces, n - 2);
    const Graph graph(n, plane_graph_edges(g));
    EXPECT_EQ(check_triconnected(graph).is_triconnected, polyhedral);
    EXPECT_EQ(check_simple_quadrangulation(graph).is_simple_quadrangulation, polyhedral);
    EXPECT_EQ(quadrangulation_is_polyhedral(g), polyhedral);
}

TEST(QuadrangulationExpansionTest, RootsIncludeEveryPseudoDoubleWheel) {
    for (int n = 8; n <= 22; n += 2) {
        const auto g = quadrangulation_pseudo_double_wheel(n);
        expect_quadrangulation(g);
        const auto canon = plane_graph_canonicalize(g);
        EXPECT_TRUE(quadrangulation_parent_code(canon.graph).empty());
    }
}

TEST(QuadrangulationExpansionTest, ExpansionsMatchConnectivityAndInvertExactly) {
    int accepted_p1 = 0, rejected_p1 = 0, accepted_p3 = 0;
    for (int n = 8; n <= 14; ++n) {
        quadrangulation_expansion_enumerate(n, [&](const PlaneGraphCanon& parent) {
            for (int v = 0; v < n; ++v) {
                if (parent.graph.rotation[v].size() < 4) continue;
                for (int first = 0; first < static_cast<int>(parent.graph.rotation[v].size()); ++first) {
                    const auto child = quadrangulation_expand_p1(parent.graph, v, first);
                    const bool allowed = quadrangulation_can_expand_p1(parent.graph, v, first);
                    // A nontrivial 3-cut can fail even though all degrees
                    // remain >= 3 and every face is still quadrilateral.
                    expect_quadrangulation(child, allowed);
                    if (!allowed) { ++rejected_p1; continue; }
                    ++accepted_p1;
                    PlaneGraph reduced;
                    ASSERT_TRUE(quadrangulation_reduce_p1(child, n, 0, reduced));
                    EXPECT_EQ(plane_graph_edges(reduced), plane_graph_edges(parent.graph));
                    EXPECT_EQ(plane_graph_canonicalize(reduced).code, parent.code);
                }
            }
            for (const auto& face : plane_graph_faces(parent.graph).boundary) {
                const auto child = quadrangulation_expand_p3(parent.graph, face);
                expect_quadrangulation(child);
                PlaneGraph reduced;
                ASSERT_TRUE(quadrangulation_reduce_p3(child, {n, n + 1, n + 2, n + 3}, reduced));
                EXPECT_EQ(plane_graph_edges(reduced), plane_graph_edges(parent.graph));
                EXPECT_EQ(plane_graph_canonicalize(reduced).code, parent.code);
                ++accepted_p3;
            }
        });
    }
    EXPECT_GT(accepted_p1, 0);
    EXPECT_GT(rejected_p1, 0);
    EXPECT_GT(accepted_p3, 0);
}

TEST(QuadrangulationExpansionTest, CanonicalSearchMatchesUnrestrictedExpansionClosure) {
    // Breadth-first closure with a global set and no canonical reductions.
    // This detects incorrect parent rejection and cross-root duplicates.
    const int limit = 16;
    std::vector<std::map<std::vector<int>, PlaneGraph>> levels(limit + 1);
    auto add = [&](const PlaneGraph& g) {
        const auto canon = plane_graph_canonicalize(g);
        levels[g.rotation.size()].emplace(canon.code, canon.graph);
    };
    for (int n = 8; n <= limit; n += 2) add(quadrangulation_pseudo_double_wheel(n));
    std::mt19937 random(20260925);
    int p3_parents = 0;
    for (int n = 8; n <= limit; ++n) {
        std::set<std::vector<int>> expected, actual;
        for (const auto& entry : levels[n]) {
            expected.insert(entry.first);
            const auto& g = entry.second;
            expect_quadrangulation(g);
            const auto parent = quadrangulation_parent_code(g);
            if (!parent.empty()) {
                int parent_n = static_cast<int>(std::count(parent.begin(), parent.end(), 0));
                EXPECT_TRUE(levels[parent_n].count(parent));
                if (parent_n == n - 4) ++p3_parents;
            }
            // Labels, cyclic starting points and orientation are immaterial.
            std::vector<int> labels(n);
            for (int v = 0; v < n; ++v) labels[v] = v;
            std::shuffle(labels.begin(), labels.end(), random);
            PlaneGraph permuted;
            permuted.rotation.resize(n);
            for (int v = 0; v < n; ++v) {
                auto& row = permuted.rotation[labels[v]];
                for (int w : g.rotation[v]) row.push_back(labels[w]);
                std::rotate(row.begin(), row.begin() + random() % row.size(), row.end());
                std::reverse(row.begin(), row.end());
            }
            const auto canon = plane_graph_canonicalize(permuted);
            EXPECT_EQ(canon.code, entry.first);
            EXPECT_EQ(quadrangulation_parent_code(canon.graph), parent);

            if (n < limit) {
                for (int v = 0; v < n; ++v) {
                    if (g.rotation[v].size() < 4) continue;
                    for (int first = 0; first < static_cast<int>(g.rotation[v].size()); ++first) {
                        const auto child = quadrangulation_expand_p1(g, v, first);
                        // Independent, general connectivity oracle.
                        if (check_triconnected(Graph(n + 1, plane_graph_edges(child))).is_triconnected)
                            add(child);
                    }
                }
            }
            if (limit - n >= 4)
                for (const auto& face : plane_graph_faces(g).boundary)
                    add(quadrangulation_expand_p3(g, face));
        }
        quadrangulation_expansion_enumerate(n, [&](const PlaneGraphCanon& g) {
            EXPECT_TRUE(actual.insert(g.code).second);
        });
        EXPECT_EQ(actual, expected) << n;
    }
    EXPECT_GT(p3_parents, 0);  // P1 alone misses the cube-in-a-face graphs.
}

TEST(QuadrangulationExpansionTest, PublishedCountsValidityAndAbstractUniqueness) {
    // Brinkmann et al., Table 3, q3(n): reflectional isomorphisms identified.
    const std::size_t counts[] = {1, 0, 1, 1, 3, 3, 11, 18, 58, 139, 451};
    for (int n = 8; n <= 18; ++n) {
        const auto result = enumerate_simple_quadrangulation_unlabeled_graphs(n);
        ASSERT_EQ(result.graphs.size(), counts[n - 8]) << n;
        std::set<std::vector<unsigned long long>> forms;
        for (const auto& graph : result.graphs) {
            EXPECT_EQ(graph.n, n);
            EXPECT_EQ(graph.edges.size(), static_cast<std::size_t>(2 * n - 4));
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            EXPECT_TRUE(check_simple_quadrangulation(Graph(n, graph.edges)).is_simple_quadrangulation);
            std::vector<unsigned long long> adj(n, 0);
            for (const auto& e : graph.edges) {
                ASSERT_GE(e.first, 1);
                ASSERT_LT(e.first, e.second);
                ASSERT_LE(e.second, n);
                adj[e.first - 1] |= 1ULL << (e.second - 1);
                adj[e.second - 1] |= 1ULL << (e.first - 1);
            }
            // Generic abstract canonicalization is independent of rotations
            // but factorial; bound this oracle, not the published count test.
            if (n <= 12) {
                EXPECT_TRUE(forms.insert(canonicalize_bitmask_graph(n, adj).form).second);
            }
        }
    }
}

TEST(QuadrangulationExpansionTest, LegacySetsDefaultsAndInputValidation) {
    for (int n = 0; n <= 9; ++n) {
        const auto current = enumerate_simple_quadrangulation_unlabeled_graphs(n);
        const auto explicit_current = enumerate_simple_quadrangulation_unlabeled_graphs(n, Algorithm::PLANTRI);
        const auto legacy = enumerate_simple_quadrangulation_unlabeled_graphs(n, Algorithm::REVERSE_SEARCH);
        std::set<Edges> actual, expected;
        ASSERT_EQ(current.graphs.size(), explicit_current.graphs.size());
        for (std::size_t i = 0; i < current.graphs.size(); ++i) {
            EXPECT_EQ(current.graphs[i].edges, explicit_current.graphs[i].edges);
            EXPECT_TRUE(actual.insert(gtest_utils::canonical_edge_list(n, current.graphs[i].edges)).second);
        }
        for (const auto& graph : legacy.graphs)
            EXPECT_TRUE(expected.insert(gtest_utils::canonical_edge_list(n, graph.edges)).second);
        EXPECT_EQ(actual, expected) << n;
    }
    for (const auto algo : {Algorithm::PLANTRI, Algorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_simple_quadrangulation_unlabeled_graphs(-1, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_simple_quadrangulation_unlabeled_graphs(INT_MAX, algo), std::invalid_argument);
    }
    for (const int n : {0, 8})
        EXPECT_THROW(enumerate_simple_quadrangulation_unlabeled_graphs(n, static_cast<Algorithm>(99)),
                     std::invalid_argument);
}
}  // namespace
