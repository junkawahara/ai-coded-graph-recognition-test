#include "enumerators/fullerene_unlabeled_enum.h"
#include "recognizers/fullerene.h"
#include <gtest/gtest.h>
#include <climits>
#include <map>
#include <random>
#include <set>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using Algorithm = FullereneUnlabeledEnumAlgorithm;
using Code = std::vector<int>;

void expect_dual_fullerene(const PlaneGraph& graph) {
    const int n = static_cast<int>(graph.rotation.size());
    int pentagons = 0;
    std::set<std::pair<int, int>> darts;
    for (int v = 0; v < n; ++v) {
        const auto& row = graph.rotation[v];
        ASSERT_TRUE(row.size() == 5 || row.size() == 6);
        pentagons += row.size() == 5;
        for (int w : row) {
            ASSERT_GE(w, 0);
            ASSERT_LT(w, n);
            ASSERT_NE(v, w);
            ASSERT_TRUE(darts.insert({v, w}).second);
            ASSERT_NE(std::find(graph.rotation[w].begin(), graph.rotation[w].end(), v),
                      graph.rotation[w].end());
        }
    }
    EXPECT_EQ(pentagons, 12);
    EXPECT_EQ(darts.size(), static_cast<std::size_t>(6 * n - 12));
    int faces = 0;
    while (!darts.empty()) {
        const auto first = *darts.begin();
        auto dart = first;
        std::set<int> vertices;
        for (int step = 0; step < 3; ++step) {
            ASSERT_EQ(darts.erase(dart), 1u);
            EXPECT_TRUE(vertices.insert(dart.first).second);
            const auto& row = graph.rotation[dart.second];
            const auto pos = std::find(row.begin(), row.end(), dart.first) - row.begin();
            dart = {dart.second, row[(pos + 1) % row.size()]};
        }
        ASSERT_EQ(dart, first);
        ++faces;
    }
    EXPECT_EQ(faces, 2 * n - 4);
    const Graph abstract(n, plane_graph_edges(graph));
    EXPECT_TRUE(check_planar(abstract).is_planar);
    EXPECT_TRUE(check_triconnected(abstract).is_triconnected);
}

TEST(FullereneExpansionTest, ExceptionalSeedsAreValidAndIrreducible) {
    std::vector<PlaneGraph> seeds{fullerene_unlabeled_c28()};
    for (int rings = 2; rings <= 6; ++rings) seeds.push_back(fullerene_unlabeled_nanotube(rings));
    for (const auto& seed : seeds) {
        expect_dual_fullerene(seed);
        const auto canonical = plane_graph_canonicalize(seed);
        EXPECT_TRUE(fullerene_unlabeled_parent_code(canonical.graph).empty());
    }
}

TEST(FullereneExpansionTest, CanonicalSearchMatchesUnrestrictedClosureAndPatchesInvert) {
    // Independent breadth-first closure: no canonical parent rejection.
    // This checks cross-parent/cross-root completeness, both expansion
    // families, mirrored operations and exact reversal of each used patch.
    const int limit = 20;  // primal orders through 36
    std::vector<std::map<Code, PlaneGraph>> levels(limit + 1);
    auto add = [&](const PlaneGraph& graph) {
        const auto canonical = plane_graph_canonicalize(graph);
        levels[graph.rotation.size()].emplace(canonical.code, canonical.graph);
    };
    add(fullerene_unlabeled_nanotube(2));
    add(fullerene_unlabeled_nanotube(3));
    add(fullerene_unlabeled_c28());
    std::mt19937 random(20260925);
    std::set<std::pair<int, int>> operations;
    int reflected = 0;
    for (int n = 12; n <= limit; ++n) {
        std::set<Code> expected, actual;
        for (const auto& entry : levels[n]) {
            expected.insert(entry.first);
            const auto& graph = entry.second;
            expect_dual_fullerene(graph);
            const auto parent = fullerene_unlabeled_parent_code(graph);
            if (!parent.empty()) {
                const auto parent_n = std::count(parent.begin(), parent.end(), 0);
                ASSERT_LT(parent_n, n);
                EXPECT_TRUE(levels[parent_n].count(parent));
            }
            // Abstract labels, rotation starting points and global reversal
            // must not change the chosen parent or the isomorphism class.
            std::vector<int> labels(n);
            for (int v = 0; v < n; ++v) labels[v] = v;
            std::shuffle(labels.begin(), labels.end(), random);
            PlaneGraph permuted;
            permuted.rotation.resize(n);
            for (int v = 0; v < n; ++v) {
                auto& row = permuted.rotation[labels[v]];
                for (int w : graph.rotation[v]) row.push_back(labels[w]);
                std::rotate(row.begin(), row.begin() + random() % row.size(), row.end());
                std::reverse(row.begin(), row.end());
            }
            const auto canonical = plane_graph_canonicalize(permuted);
            EXPECT_EQ(canonical.code, entry.first);
            EXPECT_EQ(fullerene_unlabeled_parent_code(canonical.graph), parent);

            for (int k = 2; k <= std::min(limit - n, (n - 2) / 2); ++k) {
                for (int bend = -1; bend <= k - 3; ++bend) {
                    const auto expansion = fullerene_unlabeled_expansion(k, bend);
                    for (int u = 0; u < n; ++u) {
                        for (int v : graph.rotation[u]) {
                            for (int direction : {-1, 1}) {
                                std::vector<int> mapping;
                                if (!fullerene_unlabeled_match(graph, expansion.smaller, u, v, direction, mapping)) continue;
                                PlaneGraph child, restored;
                                ASSERT_TRUE(fullerene_unlabeled_replace(graph, expansion.smaller,
                                    expansion.larger, mapping, direction, child));
                                operations.insert({k, bend});
                                reflected += direction < 0;
                                add(child);
                                for (int i = 0; i < k; ++i) mapping.push_back(n + i);
                                ASSERT_TRUE(fullerene_unlabeled_replace(child, expansion.larger,
                                    expansion.smaller, mapping, direction, restored));
                                EXPECT_EQ(plane_graph_edges(restored), plane_graph_edges(graph));
                            }
                        }
                    }
                }
            }
        }
        fullerene_unlabeled_expand(n, [&](const PlaneGraph& graph) {
            EXPECT_TRUE(actual.insert(plane_graph_canonicalize(graph).code).second);
        });
        EXPECT_EQ(actual, expected) << "dual order=" << n;
    }
    EXPECT_TRUE(operations.count({2, -1}));  // L0
    EXPECT_TRUE(operations.count({5, -1}));  // L3
    EXPECT_TRUE(operations.count({3, 0}));   // B0,0
    EXPECT_TRUE(operations.count({5, 1}));   // B1,1
    EXPECT_GT(reflected, 0);
}

TEST(FullereneExpansionTest, PublishedCountsAndDistinctValidAbstractGraphs) {
    // Brinkmann--Goedgebeur--McKay (2012), Table 2 (also A007894).
    const int counts[] = {1, 0, 1, 1, 2, 3, 6, 6, 15, 17, 40, 45, 89};
    for (int i = 0; i < static_cast<int>(sizeof(counts) / sizeof(counts[0])); ++i) {
        const int n = 20 + 2 * i;
        const auto result = enumerate_fullerene_unlabeled_graphs(n);
        ASSERT_EQ(result.graphs.size(), static_cast<std::size_t>(counts[i])) << "n=" << n;
        std::set<Code> seen;
        for (const auto& graph : result.graphs) {
            EXPECT_EQ(graph.n, n);
            EXPECT_EQ(graph.edges.size(), static_cast<std::size_t>(3 * n / 2));
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            const Graph abstract(n, graph.edges);
            EXPECT_TRUE(check_fullerene(abstract).is_fullerene);
            // Independently reconstruct the primal embedding. Exact edge
            // equality alone would not detect differently labelled copies.
            const auto embedding = compute_planar_embedding(abstract);
            ASSERT_TRUE(embedding.success);
            PlaneGraph primal;
            primal.rotation.resize(n);
            for (int v = 1; v <= n; ++v)
                for (int w : embedding.rotation[v]) primal.rotation[v - 1].push_back(w - 1);
            EXPECT_TRUE(seen.insert(plane_graph_canonicalize(primal).code).second);
        }
    }
}

TEST(FullereneExpansionTest, LegacyDualFinalizationAgreesOnCompleteCandidates) {
    // The legacy search over partial labelled graphs takes hours even for
    // C20. Exercise its unmodified leaf path on every small valid dual,
    // including duplicate rejection and its older orientation routine.
    for (int n = 12; n <= 18; ++n) {
        fullerene_unlabeled_expand(n, [&](const PlaneGraph& dual) {
            TriangEnumState state(n);
            state.alive_count = n;
            for (int v = 0; v < n; ++v) {
                state.deg[v + 1] = static_cast<int>(dual.rotation[v].size());
                for (int w : dual.rotation[v]) state.adj[v + 1][w + 1] = 1;
            }
            std::set<std::string> seen;
            std::vector<FullereneUnlabeledEnumeratedGraph> legacy;
            triang_enum_dfs(state, 2 * n - 4, &seen, &legacy);
            ASSERT_EQ(legacy.size(), 1u);
            triang_enum_dfs(state, 2 * n - 4, &seen, &legacy);
            EXPECT_EQ(legacy.size(), 1u);
            EXPECT_TRUE(check_fullerene(Graph(2 * n - 4, legacy[0].edges)).is_fullerene);
            const auto old_rotation = orient_triangulation(n, state.adj);
            PlaneGraph old_dual;
            old_dual.rotation.resize(n);
            for (int v = 1; v <= n; ++v)
                for (int w : old_rotation[v]) old_dual.rotation[v - 1].push_back(w - 1);
            EXPECT_EQ(plane_graph_canonicalize(old_dual).code, plane_graph_canonicalize(dual).code);
        });
    }
}

TEST(FullereneExpansionTest, DefaultsAndBoundaryInputs) {
    EXPECT_EQ(static_cast<int>(Algorithm::REVERSE_SEARCH), 0);
    const auto implicit = enumerate_fullerene_unlabeled_graphs(30);
    const auto explicit_new = enumerate_fullerene_unlabeled_graphs(30, Algorithm::BUCKYGEN);
    ASSERT_EQ(implicit.graphs.size(), explicit_new.graphs.size());
    for (std::size_t i = 0; i < implicit.graphs.size(); ++i)
        EXPECT_EQ(implicit.graphs[i].edges, explicit_new.graphs[i].edges);
    for (const auto algorithm : {Algorithm::BUCKYGEN, Algorithm::REVERSE_SEARCH}) {
        for (int n : {0, 1, 19, 21, 22, 23, 101})
            EXPECT_TRUE(enumerate_fullerene_unlabeled_graphs(n, algorithm).graphs.empty());
        EXPECT_THROW(enumerate_fullerene_unlabeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_fullerene_unlabeled_graphs(INT_MAX, algorithm), std::invalid_argument);
    }
}

}  // namespace
