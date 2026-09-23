#include "enumerators/bipartite_permutation_labeled_enum.h"
#include "enumerators/bipartite_permutation_unlabeled_enum.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace {

using namespace graph_recognition;
using LabeledAlgorithm = BipartitePermutationLabeledEnumAlgorithm;
using UnlabeledAlgorithm = BipartitePermutationUnlabeledEnumAlgorithm;
using Edges = std::vector<std::pair<int, int>>;

template <typename Result>
std::set<Edges> edge_sets(const Result& result, int n) {
    std::set<Edges> sets;
    for (const auto& graph : result.graphs) {
        EXPECT_EQ(graph.n, n);
        EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
        for (const auto& edge : graph.edges) {
            EXPECT_GE(edge.first, 1);
            EXPECT_LT(edge.first, edge.second);
            EXPECT_LE(edge.second, n);
        }
        EXPECT_TRUE(sets.insert(graph.edges).second);
    }
    return sets;
}

std::set<std::vector<unsigned long long>> isomorphism_classes(
    const BipartitePermutationUnlabeledEnumerationResult& result, int n, bool connected) {
    std::set<std::vector<unsigned long long>> forms;
    for (const auto& graph : result.graphs) {
        std::vector<unsigned long long> adj(n, 0);
        for (const auto& edge : graph.edges) {
            adj[edge.first - 1] |= 1ULL << (edge.second - 1);
            adj[edge.second - 1] |= 1ULL << (edge.first - 1);
        }
        if (connected) {
            EXPECT_TRUE(bitmask_graph_connected(n, adj));
        }
        const auto form = n == 0 ? std::vector<unsigned long long>()
                                 : canonicalize_bitmask_graph(n, adj).form;
        EXPECT_TRUE(forms.insert(form).second);
    }
    return forms;
}

TEST(BipartitePermutationStringEnumTest, LabeledMatchesLegacyThroughSixVertices) {
    const std::size_t counts[] = {1, 1, 2, 7, 41, 376, 5117};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_bipartite_permutation_labeled_graphs_reverse_search(
            n, LabeledAlgorithm::STRING_REPRESENTATION);
        const auto legacy = enumerate_bipartite_permutation_labeled_graphs_reverse_search(
            n, LabeledAlgorithm::REVERSE_SEARCH);
        EXPECT_EQ(dedicated.graphs.size(), counts[n]);
        EXPECT_EQ(edge_sets(dedicated, n), edge_sets(legacy, n));
        const auto defaults = enumerate_bipartite_permutation_labeled_graphs_reverse_search(n);
        ASSERT_EQ(defaults.graphs.size(), dedicated.graphs.size());
        for (std::size_t i = 0; i < defaults.graphs.size(); ++i)
            EXPECT_EQ(defaults.graphs[i].edges, dedicated.graphs[i].edges);
    }
}

TEST(BipartitePermutationStringEnumTest, UnlabeledMatchesLegacyThroughEightVertices) {
    for (bool connected : {false, true}) {
        for (int n = 0; n <= 8; ++n) {
            SCOPED_TRACE(n);
            SCOPED_TRACE(connected);
            const auto dedicated = enumerate_bipartite_permutation_unlabeled_graphs(
                n, connected, UnlabeledAlgorithm::STRING_REPRESENTATION);
            const auto legacy = enumerate_bipartite_permutation_unlabeled_graphs(
                n, connected, UnlabeledAlgorithm::CANONICAL_AUGMENTATION);
            edge_sets(dedicated, n);
            EXPECT_EQ(isomorphism_classes(dedicated, n, connected),
                      isomorphism_classes(legacy, n, connected));
            const auto defaults = enumerate_bipartite_permutation_unlabeled_graphs(n, connected);
            ASSERT_EQ(defaults.graphs.size(), dedicated.graphs.size());
            for (std::size_t i = 0; i < defaults.graphs.size(); ++i)
                EXPECT_EQ(defaults.graphs[i].edges, dedicated.graphs[i].edges);
        }
    }
}

TEST(BipartitePermutationStringEnumTest, ConnectedCountsThroughTwelveVertices) {
    // Saitoh et al., Theorem 13 (Catalan/binomial formula). Even sizes also
    // exercise equal-sized bipartitions and all four orientation symmetries.
    const std::size_t counts[] = {1, 1, 1, 1, 3, 5, 16, 38, 126, 375, 1282, 4262, 14938};
    for (int n = 0; n <= 12; ++n) {
        SCOPED_TRACE(n);
        EXPECT_EQ(enumerate_bipartite_permutation_unlabeled_graphs(n, true).graphs.size(),
                  counts[n]);
    }
}

TEST(BipartitePermutationStringEnumTest, KeepsCanonicalDescendantsOfNoncanonicalStrings) {
    // top="[[[]]]", bottom="[][][]" is canonical, but its endpoint-swap
    // parent has bottom="[][[]]" and is noncanonical. Pruning that parent's
    // subtree by symmetry loses this connected six-vertex chain graph.
    const Edges chain = {{1, 4}, {2, 4}, {2, 5}, {3, 4}, {3, 5}, {3, 6}};
    const auto result = enumerate_bipartite_permutation_unlabeled_graphs(6, true);
    EXPECT_EQ(edge_sets(result, 6).count(chain), 1u);
}

TEST(BipartitePermutationStringEnumTest, HandlesTwinsIsolatesAndRepeatedComponents) {
    const auto labeled = enumerate_bipartite_permutation_labeled_graphs_reverse_search(6);
    std::size_t stars = 0, matchings = 0, c4_and_edge = 0;
    for (const auto& graph : labeled.graphs) {
        std::vector<int> degrees(6, 0);
        for (const auto& edge : graph.edges) {
            ++degrees[edge.first - 1];
            ++degrees[edge.second - 1];
        }
        std::sort(degrees.begin(), degrees.end());
        if (degrees == std::vector<int>{1, 1, 1, 1, 1, 5}) ++stars;
        if (degrees == std::vector<int>{1, 1, 1, 1, 1, 1}) ++matchings;
        if (degrees == std::vector<int>{1, 1, 2, 2, 2, 2}) {
            std::vector<unsigned long long> adj(6, 0);
            for (const auto& edge : graph.edges) {
                adj[edge.first - 1] |= 1ULL << (edge.second - 1);
                adj[edge.second - 1] |= 1ULL << (edge.first - 1);
            }
            if (!bitmask_graph_connected(6, adj)) ++c4_and_edge;
        }
    }
    EXPECT_EQ(stars, 6u);
    EXPECT_EQ(matchings, 15u);  // Three interchangeable K2 components.
    EXPECT_EQ(c4_and_edge, 45u);
    const auto sets = edge_sets(labeled, 6);
    EXPECT_EQ(sets.count({}), 1u);
    EXPECT_EQ(sets.count({{1, 6}, {2, 5}}), 1u);  // Two isolated vertices retained.

    const auto unlabeled = enumerate_bipartite_permutation_unlabeled_graphs(8);
    const Edges two_c4 = {{1, 3}, {1, 4}, {2, 3}, {2, 4},
                         {5, 7}, {5, 8}, {6, 7}, {6, 8}};
    EXPECT_EQ(edge_sets(unlabeled, 8).count(two_c4), 1u);
}

TEST(BipartitePermutationStringEnumTest, PreservesLegacyOrderAndValidatesInputs) {
    const auto labeled = enumerate_bipartite_permutation_labeled_graphs_reverse_search(
        3, LabeledAlgorithm::REVERSE_SEARCH);
    const std::vector<Edges> labeled_order = {
        {}, {{1, 3}}, {{2, 3}}, {{1, 3}, {2, 3}},
        {{1, 2}}, {{1, 2}, {1, 3}}, {{1, 2}, {2, 3}}};
    ASSERT_EQ(labeled.graphs.size(), labeled_order.size());
    for (std::size_t i = 0; i < labeled_order.size(); ++i)
        EXPECT_EQ(labeled.graphs[i].edges, labeled_order[i]);
    const auto unlabeled = enumerate_bipartite_permutation_unlabeled_graphs(
        3, false, UnlabeledAlgorithm::CANONICAL_AUGMENTATION);
    const std::vector<Edges> unlabeled_order = {{}, {{1, 3}}, {{1, 3}, {2, 3}}};
    ASSERT_EQ(unlabeled.graphs.size(), unlabeled_order.size());
    for (std::size_t i = 0; i < unlabeled_order.size(); ++i)
        EXPECT_EQ(unlabeled.graphs[i].edges, unlabeled_order[i]);

    for (auto algo : {LabeledAlgorithm::STRING_REPRESENTATION, LabeledAlgorithm::REVERSE_SEARCH}) {
        for (int n : {-1, 65})
            EXPECT_THROW(enumerate_bipartite_permutation_labeled_graphs_reverse_search(n, algo),
                         std::invalid_argument);
    }
    for (auto algo : {UnlabeledAlgorithm::STRING_REPRESENTATION, UnlabeledAlgorithm::CANONICAL_AUGMENTATION}) {
        for (int n : {-1, 64})
            EXPECT_THROW(enumerate_bipartite_permutation_unlabeled_graphs(n, false, algo),
                         std::invalid_argument);
    }
    EXPECT_THROW(enumerate_bipartite_permutation_labeled_graphs_reverse_search(
                     0, static_cast<LabeledAlgorithm>(-1)), std::invalid_argument);
    EXPECT_THROW(enumerate_bipartite_permutation_unlabeled_graphs(
                     0, true, static_cast<UnlabeledAlgorithm>(-1)), std::invalid_argument);
}

}  // namespace
