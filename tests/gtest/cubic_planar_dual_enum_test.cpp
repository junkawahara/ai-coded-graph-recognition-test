#include "util/cubic_planar_dual_enum.h"
#include "enumerators/cubic_planar_labeled_enum.h"
#include "enumerators/cubic_planar_unlabeled_enum.h"
#include "recognizers/biconnected.h"
#include "recognizers/cubic_planar.h"
#include "recognizers/triconnected.h"
#include "test_helpers.h"
#include <gtest/gtest.h>
#include <numeric>
#include <random>

namespace {
using namespace graph_recognition;
using namespace graph_recognition::detail;
using graph_recognition::gtest_utils::canonical_edge_list;
using Edges = std::vector<std::pair<int, int>>;

TEST(CubicPlanarDualEnumTest, SphereMapsAndDualsMatchPublishedCounts) {
    // Brinkmann--McKay (2007), Table 1: t1 and t1+ count EMBEDDINGS.
    // A005964 counts ABSTRACT connected cubic planar graphs on 2k vertices.
    const int maps[] = {1, 1, 3, 9, 38, 178, 1041};
    const int simple_maps[] = {1, 1, 3, 9, 37, 172, 993};
    const int abstract[] = {1, 1, 3, 9, 32, 133, 681};
    std::mt19937 random(20260925);
    bool saw_bridge = false, saw_two_cut = false, saw_nonsimple_dual = false;
    for (int n = 4; n <= 16; n += 2) {
        int map_count = 0, simple_count = 0;
        std::set<std::vector<int>> forms;
        std::set<Edges> brute_forms;
        cubic_planar_triangulations_enumerate(n, [&](const CubicPlanarTriangulation& g) {
            ++map_count;
            ASSERT_EQ(g.next.size(), static_cast<std::size_t>(3 * n));
            std::vector<int> predecessor(g.next.size(), 0);
            for (int d = 0; d < static_cast<int>(g.next.size()); ++d) {
                ASSERT_GE(g.next[d], 0);
                ASSERT_LT(g.next[d], static_cast<int>(g.next.size()));
                ++predecessor[g.next[d]];
            }
            for (int d = 0; d < static_cast<int>(g.next.size()); ++d) {
                EXPECT_EQ(predecessor[d], 1);
                EXPECT_NE(g.next[d], d);
                EXPECT_EQ(g.next[g.next[g.next[d]]], d);
            }
            // Independently count the vertex orbits of face-next after twin.
            std::set<int> remaining;
            for (int d = 0; d < static_cast<int>(g.next.size()); ++d) remaining.insert(d);
            int vertex_count = 0;
            while (!remaining.empty()) {
                const int start = *remaining.begin();
                int d = start, degree = 0;
                do {
                    ASSERT_EQ(remaining.erase(d), 1u);
                    ++degree;
                    d = g.next[d ^ 1];
                } while (d != start);
                EXPECT_GE(degree, 3);
                ++vertex_count;
            }
            EXPECT_EQ(vertex_count - 3 * n / 2 + n, 2);  // Sphere Euler characteristic.

            // Arbitrary edge/dart renumbering and reflection preserve map codes.
            std::vector<int> edge_labels(3 * n / 2), dart_labels(3 * n);
            std::iota(edge_labels.begin(), edge_labels.end(), 0);
            std::shuffle(edge_labels.begin(), edge_labels.end(), random);
            for (std::size_t e = 0; e < edge_labels.size(); ++e) {
                dart_labels[2 * e] = 2 * edge_labels[e] + random() % 2;
                dart_labels[2 * e + 1] = dart_labels[2 * e] ^ 1;
            }
            auto permuted = g;
            for (int d = 0; d < 3 * n; ++d)
                permuted.next[dart_labels[d]] = dart_labels[
                    map_count % 2 ? g.next[d] : g.next[g.next[d]]];
            EXPECT_EQ(cubic_planar_triangulation_code(g), cubic_planar_triangulation_code(permuted));

            Edges edges;
            if (!cubic_planar_simple_dual(g, edges)) {
                saw_nonsimple_dual = true;
                return;
            }
            ++simple_count;
            const Graph graph(n, edges);
            EXPECT_TRUE(check_cubic_planar(graph).is_cubic_planar);
            EXPECT_TRUE(find_disconnected_pair(graph).empty());
            if (!check_biconnected(graph).is_biconnected) saw_bridge = true;
            else if (!check_triconnected(graph).is_triconnected) saw_two_cut = true;
            const auto code = cubic_planar_abstract_code(n, edges);
            forms.insert(code);
            if (n <= 8) brute_forms.insert(canonical_edge_list(n, edges));
            std::vector<int> labels(n);
            std::iota(labels.begin(), labels.end(), 1);
            std::shuffle(labels.begin(), labels.end(), random);
            for (auto& edge : edges) {
                edge.first = labels[edge.first - 1];
                edge.second = labels[edge.second - 1];
            }
            EXPECT_EQ(code, cubic_planar_abstract_code(n, edges));
        });
        EXPECT_EQ(map_count, maps[(n - 4) / 2]);
        EXPECT_EQ(simple_count, simple_maps[(n - 4) / 2]);
        EXPECT_EQ(forms.size(), static_cast<std::size_t>(abstract[(n - 4) / 2]));
        if (n <= 8) { EXPECT_EQ(forms.size(), brute_forms.size()); }
        EXPECT_EQ(enumerate_cubic_planar_unlabeled_graphs(n, true).graphs.size(), forms.size());
    }
    EXPECT_TRUE(saw_bridge);
    EXPECT_TRUE(saw_two_cut);
    EXPECT_TRUE(saw_nonsimple_dual);
}

std::vector<unsigned long long> independent_form(int n, const Edges& edges) {
    std::vector<unsigned long long> adj(n, 0);
    for (const auto& edge : edges) {
        adj[edge.first - 1] |= 1ULL << (edge.second - 1);
        adj[edge.second - 1] |= 1ULL << (edge.first - 1);
    }
    return canonicalize_bitmask_graph(n, adj).form;
}

TEST(CubicPlanarDualEnumTest, DedicatedAndLegacySetsAgreeIncludingDisconnectedGraphs) {
    using L = CubicPlanarLabeledEnumAlgorithm;
    using U = CubicPlanarUnlabeledEnumAlgorithm;
    for (int n = 0; n <= 10; ++n) {
        for (bool connected : {false, true}) {
            const auto dedicated = enumerate_cubic_planar_unlabeled_graphs(n, connected, U::PLANTRI);
            const auto defaults = enumerate_cubic_planar_unlabeled_graphs(n, connected);
            const auto legacy = enumerate_cubic_planar_unlabeled_graphs(n, connected, U::CANONICAL_AUGMENTATION);
            std::set<std::vector<unsigned long long>> actual, expected;
            ASSERT_EQ(dedicated.graphs.size(), defaults.graphs.size());
            for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
                const auto& g = dedicated.graphs[i];
                EXPECT_EQ(g.n, n);
                EXPECT_EQ(g.edges, defaults.graphs[i].edges);
                EXPECT_TRUE(actual.insert(independent_form(n, g.edges)).second);
            }
            for (const auto& g : legacy.graphs) expected.insert(independent_form(n, g.edges));
            EXPECT_EQ(actual, expected) << n << " connected=" << connected;
        }
        if (n > 8) continue;
        const auto dedicated = enumerate_cubic_planar_labeled_graphs(n, L::PLANTRI);
        const auto defaults = enumerate_cubic_planar_labeled_graphs(n);
        const auto legacy = enumerate_cubic_planar_labeled_graphs(n, L::REVERSE_SEARCH);
        std::set<Edges> actual, expected;
        ASSERT_EQ(dedicated.graphs.size(), defaults.graphs.size());
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            EXPECT_EQ(dedicated.graphs[i].edges, defaults.graphs[i].edges);
            EXPECT_TRUE(actual.insert(dedicated.graphs[i].edges).second);
        }
        for (const auto& g : legacy.graphs) expected.insert(g.edges);
        EXPECT_EQ(actual, expected) << n;
    }
    // Repeated isomorphic components are an unordered multiset, including 3 K4s.
    const auto twelve = enumerate_cubic_planar_unlabeled_graphs(12);
    ASSERT_EQ(twelve.graphs.size(), 37u);
    Edges three_k4s;
    for (int offset : {0, 4, 8})
        for (int u = 1; u <= 4; ++u)
            for (int v = u + 1; v <= 4; ++v) three_k4s.push_back({u + offset, v + offset});
    EXPECT_EQ(std::count_if(twelve.graphs.begin(), twelve.graphs.end(),
        [&](const CubicPlanarUnlabeledEnumeratedGraph& g) { return g.edges == three_k4s; }), 1);
}

TEST(CubicPlanarDualEnumTest, BoundsAndInvalidAlgorithms) {
    using L = CubicPlanarLabeledEnumAlgorithm;
    using U = CubicPlanarUnlabeledEnumAlgorithm;
    for (auto algorithm : {L::PLANTRI, L::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_cubic_planar_labeled_graphs(-1, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_cubic_planar_labeled_graphs(max_vertex_count + 1, algorithm), std::invalid_argument);
        for (int n : {0, 1, 2, 3, 5, 63}) EXPECT_TRUE(enumerate_cubic_planar_labeled_graphs(n, algorithm).graphs.empty());
    }
    EXPECT_THROW(enumerate_cubic_planar_labeled_graphs(64), std::invalid_argument);
    EXPECT_TRUE(enumerate_cubic_planar_labeled_graphs(65, L::REVERSE_SEARCH).graphs.empty());
    EXPECT_TRUE(enumerate_cubic_planar_labeled_graphs(max_vertex_count - 1, L::REVERSE_SEARCH).graphs.empty());
    for (auto algorithm : {U::PLANTRI, U::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_cubic_planar_unlabeled_graphs(-1, false, algorithm), std::invalid_argument);
        EXPECT_THROW(enumerate_cubic_planar_unlabeled_graphs(64, false, algorithm), std::invalid_argument);
        EXPECT_TRUE(enumerate_cubic_planar_unlabeled_graphs(63, false, algorithm).graphs.empty());
    }
    EXPECT_THROW(enumerate_cubic_planar_labeled_graphs(0, static_cast<L>(99)), std::invalid_argument);
    EXPECT_THROW(enumerate_cubic_planar_unlabeled_graphs(0, false, static_cast<U>(99)), std::invalid_argument);
}
}  // namespace
