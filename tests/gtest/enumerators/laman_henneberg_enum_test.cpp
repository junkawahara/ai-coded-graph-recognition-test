#include "enumerators/laman_labeled_enum.h"
#include "enumerators/laman_unlabeled_enum.h"
#include "recognizers/laman.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

using namespace graph_recognition;

namespace {

using Labeled = LamanLabeledEnumAlgorithm;
using Unlabeled = LamanUnlabeledEnumAlgorithm;
using Edges = std::vector<std::pair<int, int>>;

template <typename Result>
std::set<Edges> edge_sets(const Result& result, bool canonical = false) {
    std::set<Edges> sets;
    for (const auto& graph : result.graphs) {
        EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
        for (const auto& edge : graph.edges) {
            EXPECT_GE(edge.first, 1);
            EXPECT_LT(edge.first, edge.second);
            EXPECT_LE(edge.second, graph.n);
        }
        const auto edges = canonical
            ? gtest_utils::canonical_edge_list(graph.n, graph.edges) : graph.edges;
        EXPECT_TRUE(sets.insert(edges).second) << "duplicate at n=" << graph.n;
    }
    return sets;
}

TEST(LamanHennebergEnumTest, LabeledOutputEqualsLegacyThroughSixVertices) {
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto actual = enumerate_laman_labeled_graphs(n);
        const auto legacy = enumerate_laman_labeled_graphs(n, Labeled::REVERSE_SEARCH);
        const auto explicit_algo = enumerate_laman_labeled_graphs(n, Labeled::HENNEBERG);
        EXPECT_EQ(edge_sets(actual), edge_sets(legacy));
        ASSERT_EQ(actual.graphs.size(), explicit_algo.graphs.size());
        for (std::size_t i = 0; i < actual.graphs.size(); ++i) {
            EXPECT_EQ(actual.graphs[i].n, n);
            EXPECT_EQ(actual.graphs[i].edges, explicit_algo.graphs[i].edges);
        }
    }
}

TEST(LamanHennebergEnumTest, UnlabeledClassesEqualLegacyThroughSevenVertices) {
    for (int n = 0; n <= 7; ++n) {
        SCOPED_TRACE(n);
        const auto actual = enumerate_laman_unlabeled_graphs(n);
        const auto legacy = enumerate_laman_unlabeled_graphs(n, Unlabeled::CANONICAL_AUGMENTATION);
        const auto explicit_algo = enumerate_laman_unlabeled_graphs(n, Unlabeled::HENNEBERG);
        EXPECT_EQ(edge_sets(actual, true), edge_sets(legacy, true));
        ASSERT_EQ(actual.graphs.size(), explicit_algo.graphs.size());
        for (std::size_t i = 0; i < actual.graphs.size(); ++i) {
            EXPECT_EQ(actual.graphs[i].n, n);
            EXPECT_EQ(actual.graphs[i].edges, explicit_algo.graphs[i].edges);
        }
    }
}

TEST(LamanHennebergEnumTest, MatchesAllSixVertexGraphsAcceptedByPebbleGame) {
    // Independent exhaustive oracle: all 15 possible edges, filtered by
    // tightness and the pebble-game recognizer rather than construction history.
    Edges possible;
    for (int u = 1; u <= 6; ++u)
        for (int v = u + 1; v <= 6; ++v) possible.emplace_back(u, v);
    std::set<Edges> expected;
    for (unsigned int mask = 0; mask < (1U << possible.size()); ++mask) {
        Edges edges;
        for (std::size_t i = 0; i < possible.size(); ++i)
            if ((mask >> i) & 1U) edges.push_back(possible[i]);
        if (edges.size() == 9 && check_laman(Graph(6, edges)).is_laman)
            expected.insert(edges);
    }
    EXPECT_EQ(expected.size(), 3355u);
    EXPECT_EQ(edge_sets(enumerate_laman_labeled_graphs(6)), expected);
}

TEST(LamanHennebergEnumTest, IncludesAllLabelingsOfGraphsRequiringHennebergII) {
    // K3,3 and the triangular prism have minimum degree 3, so a generator
    // that uses only Henneberg I cannot reach them. K3,3 has 10 labelings.
    const auto labeled = edge_sets(enumerate_laman_labeled_graphs(6));
    std::set<Edges> k33_labelings;
    for (int a = 2; a <= 6; ++a) {
        for (int b = a + 1; b <= 6; ++b) {
            Edges edges;
            for (int u = 1; u <= 6; ++u) {
                for (int v = u + 1; v <= 6; ++v) {
                    const bool left_u = u == 1 || u == a || u == b;
                    const bool left_v = v == 1 || v == a || v == b;
                    if (left_u != left_v) edges.emplace_back(u, v);
                }
            }
            k33_labelings.insert(edges);
            EXPECT_EQ(labeled.count(edges), 1u);
        }
    }
    EXPECT_EQ(k33_labelings.size(), 10u);
    const Edges prism = {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 5},
                         {3, 6}, {4, 5}, {4, 6}, {5, 6}};
    EXPECT_EQ(labeled.count(prism), 1u);

    // The largest label has degree 5: it cannot be the last Henneberg
    // vertex. Construction order must not restrict the final labeling.
    const Edges high_label_hub = {{1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6},
                                  {2, 6}, {3, 6}, {4, 6}, {5, 6}};
    EXPECT_EQ(labeled.count(high_label_hub), 1u);
}

TEST(LamanHennebergEnumTest, SmallInputsAndInvalidArguments) {
    for (const auto algo : {Labeled::HENNEBERG, Labeled::REVERSE_SEARCH}) {
        EXPECT_TRUE(enumerate_laman_labeled_graphs(0, algo).graphs.empty());
        const auto k1 = enumerate_laman_labeled_graphs(1, algo);
        ASSERT_EQ(k1.graphs.size(), 1u);
        EXPECT_EQ(k1.graphs[0].n, 1);
        EXPECT_TRUE(k1.graphs[0].edges.empty());
        const auto k2 = enumerate_laman_labeled_graphs(2, algo);
        ASSERT_EQ(k2.graphs.size(), 1u);
        EXPECT_EQ(k2.graphs[0].n, 2);
        EXPECT_EQ(k2.graphs[0].edges, (Edges{{1, 2}}));
        for (int n : {-1, 65, INT_MAX})
            EXPECT_THROW(enumerate_laman_labeled_graphs(n, algo), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_laman_labeled_graphs(64), std::invalid_argument);
    for (const auto algo : {Unlabeled::HENNEBERG, Unlabeled::CANONICAL_AUGMENTATION}) {
        EXPECT_TRUE(enumerate_laman_unlabeled_graphs(0, algo).graphs.empty());
        for (int n : {-1, 64, INT_MAX})
            EXPECT_THROW(enumerate_laman_unlabeled_graphs(n, algo), std::invalid_argument);
    }
    for (int n : {0, 1, 2, 3}) {
        EXPECT_THROW(enumerate_laman_labeled_graphs(n, static_cast<Labeled>(-1)), std::invalid_argument);
        EXPECT_THROW(enumerate_laman_unlabeled_graphs(n, static_cast<Unlabeled>(-1)), std::invalid_argument);
    }
}

}  // namespace
