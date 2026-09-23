#include "recognizers/distance_hereditary.h"
#include "enumerators/distance_hereditary_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::DistanceHereditaryLabeledEnumerationResult;
using graph_recognition::DistanceHereditaryLabeledEnumAlgorithm;
using graph_recognition::check_distance_hereditary;
using graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "distance_hereditary_labeled_enum";

class DistanceHereditaryLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DistanceHereditaryLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    DistanceHereditaryLabeledEnumerationResult res = enumerate_distance_hereditary_labeled_graphs_reverse_search(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int>>> seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int>> key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_distance_hereditary(g).is_distance_hereditary) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DistanceHereditaryLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(DistanceHereditaryLabeledEnumAlgorithmTest, MatchesLegacySetsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 64, 892, 18344};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_distance_hereditary_labeled_graphs_reverse_search(
            n, DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL);
        const auto legacy = enumerate_distance_hereditary_labeled_graphs_reverse_search(
            n, DistanceHereditaryLabeledEnumAlgorithm::REVERSE_SEARCH);
        std::set<std::vector<std::pair<int, int>>> dedicated_set, legacy_set;
        EXPECT_EQ(dedicated.graphs.size(), expected[n]);
        EXPECT_EQ(legacy.graphs.size(), expected[n]);
        for (const auto& graph : dedicated.graphs) {
            EXPECT_EQ(graph.n, n);
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            for (const auto& edge : graph.edges) {
                EXPECT_GE(edge.first, 1);
                EXPECT_LT(edge.first, edge.second);
                EXPECT_LE(edge.second, n);
            }
            EXPECT_TRUE(dedicated_set.insert(graph.edges).second);
        }
        for (const auto& graph : legacy.graphs) {
            EXPECT_TRUE(legacy_set.insert(graph.edges).second);
        }
        EXPECT_EQ(dedicated_set, legacy_set);
        if (n == 5) {
            // The largest label is neither pendant nor a twin: increasing-label
            // pendant/twin additions alone would miss this labeling of P5.
            EXPECT_EQ(dedicated_set.count({{1, 2}, {2, 5}, {3, 4}, {3, 5}}), 1u);
        }
    }
}

TEST(DistanceHereditaryLabeledEnumAlgorithmTest, LabelExpansionHandlesTwinsAndRepeatedComponents) {
    using graph_recognition::EnumeratedGraph;
    const std::vector<std::pair<EnumeratedGraph, std::size_t>> cases = {
        // Pendant vertices, false twins, true twins, and repeated C4 components.
        {{5, {{1, 2}, {1, 3}, {1, 4}, {1, 5}}}, 5},
        {{5, {{1, 3}, {1, 4}, {1, 5}, {2, 3}, {2, 4}, {2, 5}}}, 10},
        {{4, {{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}}}, 6},
        {{8, {{1, 2}, {1, 4}, {2, 3}, {3, 4}, {5, 6}, {5, 8}, {6, 7}, {7, 8}}}, 315},
        {{6, {{1, 2}, {3, 4}}}, 45}
    };
    // Append multiple disjoint orbits to the same result, as the enumerator does.
    std::vector<EnumeratedGraph> graphs;
    for (const auto& test : cases) {
        const std::size_t first = graphs.size();
        graph_recognition::detail::expand_graph_labelings(test.first.n, test.first.edges, &graphs);
        EXPECT_EQ(graphs.size() - first, test.second);
        std::set<std::vector<std::pair<int, int>>> seen;
        for (std::size_t i = first; i < graphs.size(); ++i) {
            EXPECT_EQ(graphs[i].n, test.first.n);
            EXPECT_TRUE(seen.insert(graphs[i].edges).second);
            EXPECT_TRUE(check_distance_hereditary(Graph(graphs[i].n, graphs[i].edges))
                            .is_distance_hereditary);
        }
    }
}

TEST(DistanceHereditaryLabeledEnumAlgorithmTest, DefaultsToVertexIncrementalAndPreservesLegacyOrder) {
    const auto default_result = enumerate_distance_hereditary_labeled_graphs_reverse_search(3);
    const auto dedicated = enumerate_distance_hereditary_labeled_graphs_reverse_search(
        3, DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL);
    const auto legacy = enumerate_distance_hereditary_labeled_graphs_reverse_search(
        3, DistanceHereditaryLabeledEnumAlgorithm::REVERSE_SEARCH);
    ASSERT_EQ(default_result.graphs.size(), 8u);
    ASSERT_EQ(dedicated.graphs.size(), 8u);
    ASSERT_EQ(legacy.graphs.size(), 8u);
    EXPECT_FALSE(default_result.graphs[0].edges.empty());
    const std::vector<std::vector<std::pair<int, int>>> legacy_order = {
        {}, {{1, 3}}, {{2, 3}}, {{1, 3}, {2, 3}}, {{1, 2}},
        {{1, 2}, {1, 3}}, {{1, 2}, {2, 3}}, {{1, 2}, {1, 3}, {2, 3}}
    };
    for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
        EXPECT_EQ(legacy.graphs[i].edges, legacy_order[i]);
    }
}

TEST(DistanceHereditaryLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL,
                      DistanceHereditaryLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_distance_hereditary_labeled_graphs_reverse_search(-1, algo),
                     std::invalid_argument);
        EXPECT_THROW(enumerate_distance_hereditary_labeled_graphs_reverse_search(65, algo),
                     std::invalid_argument);
    }
    EXPECT_THROW(enumerate_distance_hereditary_labeled_graphs_reverse_search(64),
                 std::invalid_argument);
    EXPECT_THROW(enumerate_distance_hereditary_labeled_graphs_reverse_search(
                     64, DistanceHereditaryLabeledEnumAlgorithm::VERTEX_INCREMENTAL),
                 std::invalid_argument);
    EXPECT_THROW(enumerate_distance_hereditary_labeled_graphs_reverse_search(
                     0, static_cast<DistanceHereditaryLabeledEnumAlgorithm>(-1)),
                 std::invalid_argument);
}

}  // namespace
