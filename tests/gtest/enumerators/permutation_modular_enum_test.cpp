#include "enumerators/permutation_labeled_enum.h"
#include "enumerators/permutation_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <map>
#include <numeric>
#include <random>
#include <set>
#include <vector>

namespace {

using namespace graph_recognition;
using Edges = std::vector<std::pair<int, int>>;
using gtest_utils::canonical_edge_list;

Edges inversion_edges(const std::vector<int>& pi) {
    Edges edges;
    for (int u = 0; u < static_cast<int>(pi.size()); ++u)
        for (int v = u + 1; v < static_cast<int>(pi.size()); ++v)
            if (pi[u] > pi[v]) edges.emplace_back(u + 1, v + 1);
    return edges;
}

Edges relabel(const Edges& edges, const std::vector<int>& labels) {
    Edges result;
    for (const auto& edge : edges) {
        int u = labels[edge.first - 1], v = labels[edge.second - 1];
        result.emplace_back(std::min(u, v), std::max(u, v));
    }
    std::sort(result.begin(), result.end());
    return result;
}

TEST(PermutationModularCanonicalTest, ExhaustiveDiagramsAgreeWithBruteForceIsomorphism) {
    const int counts[] = {1, 1, 2, 4, 11, 33, 142};
    std::mt19937 random(2047);
    for (int n = 0; n <= 6; ++n) {
        std::vector<int> pi(n), labels(n);
        std::iota(pi.begin(), pi.end(), 1);
        std::iota(labels.begin(), labels.end(), 1);
        std::map<Edges, std::vector<int>> class_to_string;
        std::set<std::vector<int>> strings;
        do {
            const Edges edges = inversion_edges(pi);
            const auto canonical = detail::permutation_canonical_string(Graph(n, edges));
            const Edges brute = canonical_edge_list(n, edges);
            EXPECT_EQ(canonical.size(), static_cast<std::size_t>(n));
            EXPECT_EQ(canonical_edge_list(n, inversion_edges(canonical)), brute);
            const auto inserted = class_to_string.emplace(brute, canonical);
            EXPECT_EQ(inserted.first->second, canonical) << "n=" << n;
            strings.insert(canonical);
            std::shuffle(labels.begin(), labels.end(), random);
            EXPECT_EQ(detail::permutation_canonical_string(Graph(n, relabel(edges, labels))),
                      canonical) << "n=" << n;
        } while (std::next_permutation(pi.begin(), pi.end()));
        EXPECT_EQ(class_to_string.size(), static_cast<std::size_t>(counts[n]));
        EXPECT_EQ(strings.size(), class_to_string.size());
    }
}

TEST(PermutationModularCanonicalTest, PrimeDiagramsAndNestedModulesIgnoreVertexLabels) {
    // The first prime has four distinct diagram permutations. P4 inflated by
    // equal-sized but non-isomorphic children tests child-aware symmetries;
    // nesting it under series/parallel modules also tests repeated components.
    std::vector<std::vector<int>> diagrams = {
        {2, 5, 4, 1, 3}, {3, 5, 2, 1, 4}, {4, 1, 5, 3, 2}, {4, 3, 1, 5, 2},
        {3, 4, 8, 7, 2, 1, 5, 6},
        {11, 12, 16, 15, 10, 9, 13, 14, 3, 4, 8, 7, 2, 1, 5, 6},
        {3, 4, 8, 7, 2, 1, 5, 6, 11, 12, 16, 15, 10, 9, 13, 14}
    };
    std::mt19937 random(2063);
    std::vector<int> prime_form;
    for (std::size_t i = 0; i < diagrams.size(); ++i) {
        const int n = static_cast<int>(diagrams[i].size());
        const Edges edges = inversion_edges(diagrams[i]);
        const auto expected = detail::permutation_canonical_string(Graph(n, edges));
        if (i == 0) prime_form = expected;
        if (i < 4) {
            EXPECT_EQ(expected, prime_form);
        }
        std::vector<int> labels(n);
        std::iota(labels.begin(), labels.end(), 1);
        for (int trial = 0; trial < 40; ++trial) {
            std::shuffle(labels.begin(), labels.end(), random);
            EXPECT_EQ(detail::permutation_canonical_string(Graph(n, relabel(edges, labels))),
                      expected) << "diagram=" << i << " trial=" << trial;
        }
        EXPECT_EQ(detail::permutation_canonical_string(Graph(n, inversion_edges(expected))),
                  expected);
    }
}

TEST(PermutationModularEnumTest, LabeledMatchesLegacyIncludingAutomorphismDuplicates) {
    for (int n = 0; n <= 6; ++n) {
        const auto actual = enumerate_permutation_labeled_graphs_reverse_search(n);
        const auto expected = enumerate_permutation_labeled_graphs_reverse_search(
            n, PermutationLabeledEnumAlgorithm::REVERSE_SEARCH);
        std::set<Edges> actual_set, expected_set;
        for (const auto& g : actual.graphs) {
            EXPECT_EQ(g.n, n);
            EXPECT_TRUE(std::is_sorted(g.edges.begin(), g.edges.end()));
            EXPECT_TRUE(actual_set.insert(g.edges).second) << "n=" << n;
        }
        for (const auto& g : expected.graphs) expected_set.insert(g.edges);
        EXPECT_EQ(actual_set, expected_set) << "n=" << n;
    }
}

TEST(PermutationModularEnumTest, EightVertexCounts) {
    EXPECT_EQ(enumerate_permutation_unlabeled_graphs(8).graphs.size(), 5699u);
    EXPECT_EQ(enumerate_permutation_unlabeled_graphs(8, true).graphs.size(), 4753u);
}

TEST(PermutationModularEnumTest, UnlabeledMatchesLegacyInBothConnectivityModes) {
    for (int n = 0; n <= 7; ++n) {
        for (bool connected : {false, true}) {
            const auto actual = enumerate_permutation_unlabeled_graphs(n, connected);
            const auto expected = enumerate_permutation_unlabeled_graphs(
                n, connected, PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
            std::set<Edges> actual_set, expected_set;
            // This oracle tries every labeling independently of the modular
            // canonicalizer under test, so equal counts alone cannot pass.
            for (const auto& g : actual.graphs) {
                EXPECT_EQ(g.n, n);
                EXPECT_TRUE(check_permutation(Graph(n, g.edges)).is_permutation);
                EXPECT_TRUE(actual_set.insert(canonical_edge_list(n, g.edges)).second);
            }
            for (const auto& g : expected.graphs)
                expected_set.insert(canonical_edge_list(n, g.edges));
            EXPECT_EQ(actual_set, expected_set) << "n=" << n << " connected=" << connected;
            EXPECT_EQ(actual.graphs.size(), expected.graphs.size());
        }
    }
}

TEST(PermutationModularEnumTest, AlgorithmValidationAndLegacyOrder) {
    for (auto algo : {PermutationLabeledEnumAlgorithm::MODULAR_DECOMPOSITION,
                      PermutationLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_permutation_labeled_graphs_reverse_search(-1, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_permutation_labeled_graphs_reverse_search(65, algo), std::invalid_argument);
    }
    for (auto algo : {PermutationUnlabeledEnumAlgorithm::MODULAR_DECOMPOSITION,
                      PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION}) {
        EXPECT_THROW(enumerate_permutation_unlabeled_graphs(-1, false, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_permutation_unlabeled_graphs(64, false, algo), std::invalid_argument);
    }
    for (int n : {0, 3}) {
        EXPECT_THROW(enumerate_permutation_labeled_graphs_reverse_search(
            n, static_cast<PermutationLabeledEnumAlgorithm>(-1)), std::invalid_argument);
        EXPECT_THROW(enumerate_permutation_unlabeled_graphs(
            n, false, static_cast<PermutationUnlabeledEnumAlgorithm>(-1)), std::invalid_argument);
    }
    const auto legacy_labeled = enumerate_permutation_labeled_graphs_reverse_search(
        3, PermutationLabeledEnumAlgorithm::REVERSE_SEARCH);
    const std::vector<Edges> labeled_order = {
        {}, {{1, 3}}, {{2, 3}}, {{1, 3}, {2, 3}}, {{1, 2}},
        {{1, 2}, {1, 3}}, {{1, 2}, {2, 3}}, {{1, 2}, {1, 3}, {2, 3}}
    };
    ASSERT_EQ(legacy_labeled.graphs.size(), labeled_order.size());
    for (std::size_t i = 0; i < labeled_order.size(); ++i)
        EXPECT_EQ(legacy_labeled.graphs[i].edges, labeled_order[i]);
    const auto legacy_unlabeled = enumerate_permutation_unlabeled_graphs(
        3, false, PermutationUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION);
    const std::vector<Edges> unlabeled_order = {
        {}, {{1, 3}}, {{1, 3}, {2, 3}}, {{1, 2}, {1, 3}, {2, 3}}
    };
    ASSERT_EQ(legacy_unlabeled.graphs.size(), unlabeled_order.size());
    for (std::size_t i = 0; i < unlabeled_order.size(); ++i)
        EXPECT_EQ(legacy_unlabeled.graphs[i].edges, unlabeled_order[i]);

    // The new default follows edge deletions from the complete graph.
    EXPECT_EQ(enumerate_permutation_labeled_graphs_reverse_search(3).graphs.front().edges,
              labeled_order.back());
    EXPECT_EQ(enumerate_permutation_unlabeled_graphs(3).graphs.front().edges,
              unlabeled_order.back());
}

}  // namespace
