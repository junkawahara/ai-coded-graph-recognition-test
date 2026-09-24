#include "enumerators/biconnected_labeled_enum.h"
#include "enumerators/biconnected_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <climits>
#include <set>
#include <vector>

namespace {
using namespace graph_recognition;
using Labeled = BiconnectedLabeledEnumAlgorithm;
using Unlabeled = BiconnectedUnlabeledEnumAlgorithm;
using Edges = std::vector<std::pair<int, int>>;

template <typename Result>
std::set<Edges> edge_sets(const Result& result, int limit = INT_MAX, bool canonical = false) {
    std::set<Edges> keys;
    for (const auto& graph : result.graphs) {
        if (static_cast<int>(graph.edges.size()) > limit) continue;
        EXPECT_TRUE(keys.insert(canonical
            ? gtest_utils::canonical_edge_list(graph.n, graph.edges) : graph.edges).second);
    }
    return keys;
}

std::vector<unsigned long long> adjacency(int n, const Edges& edges) {
    std::vector<unsigned long long> adj(n, 0);
    for (const auto& edge : edges) {
        adj[edge.first - 1] |= 1ULL << (edge.second - 1);
        adj[edge.second - 1] |= 1ULL << (edge.first - 1);
    }
    return adj;
}

TEST(BiconnectedEarEnumTest, LabeledOutputEqualsLegacyThroughSixVertices) {
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto actual = enumerate_biconnected_labeled_graphs(n);
        const auto legacy = enumerate_biconnected_labeled_graphs(n, Labeled::REVERSE_SEARCH);
        EXPECT_EQ(edge_sets(actual), edge_sets(legacy));
        const auto explicit_algo = enumerate_biconnected_labeled_graphs(n, Labeled::EAR_AUGMENTATION);
        ASSERT_EQ(actual.graphs.size(), explicit_algo.graphs.size());
        for (std::size_t i = 0; i < actual.graphs.size(); ++i) {
            EXPECT_EQ(actual.graphs[i].n, n);
            EXPECT_EQ(actual.graphs[i].edges, explicit_algo.graphs[i].edges);
        }
    }
}

TEST(BiconnectedEarEnumTest, UnlabeledClassesEqualLegacyThroughSevenVertices) {
    for (int n = 0; n <= 7; ++n) {
        SCOPED_TRACE(n);
        const auto actual = enumerate_biconnected_unlabeled_graphs(n);
        const auto legacy = enumerate_biconnected_unlabeled_graphs(n, Unlabeled::CANONICAL_AUGMENTATION);
        EXPECT_EQ(edge_sets(actual, INT_MAX, true), edge_sets(legacy, INT_MAX, true));
    }
}

TEST(BiconnectedEarEnumTest, EdgeBoundsEqualFilteringUnrestrictedLegacy) {
    for (int n = 3; n <= 6; ++n) {
        const auto legacy = enumerate_biconnected_unlabeled_graphs(n, Unlabeled::CANONICAL_AUGMENTATION);
        for (int limit = 0; limit <= n * (n - 1) / 2 + 1; ++limit) {
            SCOPED_TRACE(::testing::Message() << "n=" << n << " limit=" << limit);
            const auto expected = edge_sets(legacy, limit, true);
            for (auto algo : {Unlabeled::EAR_AUGMENTATION, Unlabeled::CANONICAL_AUGMENTATION})
                EXPECT_EQ(edge_sets(enumerate_biconnected_unlabeled_graphs(n, algo, limit), INT_MAX, true), expected);
        }
    }
    const auto legacy = enumerate_biconnected_labeled_graphs(5, Labeled::REVERSE_SEARCH);
    for (int limit = 0; limit <= 11; ++limit)
        for (auto algo : {Labeled::EAR_AUGMENTATION, Labeled::REVERSE_SEARCH})
            EXPECT_EQ(edge_sets(enumerate_biconnected_labeled_graphs(5, algo, limit)), edge_sets(legacy, limit));
    EXPECT_EQ(enumerate_biconnected_labeled_graphs(6, Labeled::EAR_AUGMENTATION, 6).graphs.size(), 60u);
}

TEST(BiconnectedEarEnumTest, DeletionSkipsEarsThatCreateCutVertices) {
    // Two triangles sharing vertex 3, made biconnected by edge 2--4.
    const auto adj = adjacency(5, {{1, 2}, {1, 3}, {2, 3}, {3, 4}, {3, 5}, {4, 5}, {2, 4}});
    const detail::BiconnectedEar bad = {1, 3, 0, 0};
    const auto broken = detail::biconnected_ear_delete(adj, bad);
    EXPECT_FALSE(check_biconnected(Graph(5, bitmask_graph_edges(5, broken))).is_biconnected);
    const auto form = canonicalize_bitmask_graph(5, adj).form;
    const auto parent = detail::biconnected_ear_graph_from_form(detail::biconnected_ear_parent(form));
    const int size = static_cast<int>(parent.size());
    EXPECT_TRUE(check_biconnected(Graph(size, bitmask_graph_edges(size, parent))).is_biconnected);
    EXPECT_LT(bitmask_graph_edges(size, parent).size(), 7u);
}

TEST(BiconnectedEarEnumTest, CanonicalDeletionHandlesTrivialAndNontrivialEars) {
    // In a diamond the shortest deletable ear is its shared edge, leaving C4.
    const auto diamond = canonicalize_bitmask_graph(4,
        adjacency(4, {{1, 2}, {1, 3}, {2, 3}, {1, 4}, {2, 4}})).form;
    const auto c4 = canonicalize_bitmask_graph(4,
        adjacency(4, {{1, 2}, {2, 3}, {3, 4}, {1, 4}})).form;
    EXPECT_EQ(detail::biconnected_ear_parent(diamond), c4);
    // K2,3 has three interchangeable order-one ears and no trivial ears.
    const auto theta = canonicalize_bitmask_graph(5,
        adjacency(5, {{1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}})).form;
    EXPECT_EQ(detail::biconnected_ear_parent(theta), c4);
    // All cycle roots, including C_n, must be included. At m=n only C_n fits.
    for (int n = 3; n <= 9; ++n) {
        const auto result = enumerate_biconnected_unlabeled_graphs(n, Unlabeled::EAR_AUGMENTATION, n);
        ASSERT_EQ(result.graphs.size(), 1u);
        EXPECT_EQ(result.graphs[0].n, n);
        EXPECT_EQ(result.graphs[0].edges.size(), static_cast<std::size_t>(n));
    }
}

TEST(BiconnectedEarEnumTest, InvalidArgumentsAndLegacyLimits) {
    for (auto algo : {Labeled::EAR_AUGMENTATION, Labeled::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_biconnected_labeled_graphs(-1, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_biconnected_labeled_graphs(65, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_biconnected_labeled_graphs(0, algo, -2), std::invalid_argument);
        EXPECT_TRUE(enumerate_biconnected_labeled_graphs(63, algo, 0).graphs.empty());
    }
    EXPECT_THROW(enumerate_biconnected_labeled_graphs(64), std::invalid_argument);
    EXPECT_TRUE(enumerate_biconnected_labeled_graphs(64, Labeled::REVERSE_SEARCH, 0).graphs.empty());
    for (auto algo : {Unlabeled::EAR_AUGMENTATION, Unlabeled::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_biconnected_unlabeled_graphs(-1, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_biconnected_unlabeled_graphs(64, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_biconnected_unlabeled_graphs(0, algo, -2), std::invalid_argument);
        EXPECT_TRUE(enumerate_biconnected_unlabeled_graphs(63, algo, 0).graphs.empty());
        EXPECT_EQ(enumerate_biconnected_unlabeled_graphs(4, algo, INT_MAX).graphs.size(), 3u);
    }
    EXPECT_THROW(enumerate_biconnected_labeled_graphs(0, static_cast<Labeled>(-1)), std::invalid_argument);
    EXPECT_THROW(enumerate_biconnected_unlabeled_graphs(0, static_cast<Unlabeled>(-1)), std::invalid_argument);
}
}  // namespace
