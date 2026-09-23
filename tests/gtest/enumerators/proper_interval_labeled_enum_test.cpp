#include "recognizers/proper_interval.h"
#include "enumerators/proper_interval_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ProperIntervalLabeledEnumerationResult;
using graph_recognition::ProperIntervalLabeledEnumAlgorithm;
using graph_recognition::check_proper_interval;
using graph_recognition::enumerate_proper_interval_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "proper_interval_labeled_enum";

class ProperIntervalLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ProperIntervalLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ProperIntervalLabeledEnumerationResult res = enumerate_proper_interval_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_proper_interval(g).is_proper_interval) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ProperIntervalLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(ProperIntervalLabeledEnumAlgorithmTest, MatchesLegacySetsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 57, 637, 9653};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_proper_interval_labeled_graphs_reverse_search(
            n, ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION);
        const auto legacy = enumerate_proper_interval_labeled_graphs_reverse_search(
            n, ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
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
    }
}

TEST(ProperIntervalLabeledEnumAlgorithmTest, LabelExpansionHandlesAutomorphisms) {
    using graph_recognition::ProperIntervalUnlabeledEnumeratedGraph;
    const std::vector<std::pair<ProperIntervalUnlabeledEnumeratedGraph, std::size_t>> cases = {
        // Reflection of a path, true twins of a diamond, repeated components,
        // and interchangeable isolated vertices give different automorphisms.
        {{4, {{1, 2}, {2, 3}, {3, 4}}}, 12},
        {{4, {{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}}}, 6},
        {{6, {{1, 2}, {1, 3}, {2, 3}, {4, 5}, {4, 6}, {5, 6}}}, 10},
        {{6, {{1, 2}, {2, 3}, {4, 5}, {5, 6}}}, 90},
        {{6, {{1, 2}, {1, 3}, {2, 3}}}, 20}
    };
    for (const auto& test : cases) {
        std::vector<graph_recognition::EnumeratedGraph> graphs;
        graph_recognition::detail::proper_interval_labeled_expand(test.first, &graphs);
        EXPECT_EQ(graphs.size(), test.second);
        std::set<std::vector<std::pair<int, int>>> seen;
        for (const auto& graph : graphs) EXPECT_TRUE(seen.insert(graph.edges).second);
    }

    // Each has one labeling: expansion must not walk all 12! permutations.
    ProperIntervalUnlabeledEnumeratedGraph symmetric = {12, {}};
    for (int complete = 0; complete <= 1; ++complete) {
        if (complete) {
            for (int u = 1; u <= symmetric.n; ++u)
                for (int v = u + 1; v <= symmetric.n; ++v)
                    symmetric.edges.push_back({u, v});
        }
        std::vector<graph_recognition::EnumeratedGraph> graphs;
        graph_recognition::detail::proper_interval_labeled_expand(symmetric, &graphs);
        ASSERT_EQ(graphs.size(), 1u);
        EXPECT_EQ(graphs[0].n, symmetric.n);
        EXPECT_EQ(graphs[0].edges, symmetric.edges);
    }
}

TEST(ProperIntervalLabeledEnumAlgorithmTest, DefaultsToStringsAndPreservesLegacyOrder) {
    const auto default_result = enumerate_proper_interval_labeled_graphs_reverse_search(3);
    const auto dedicated = enumerate_proper_interval_labeled_graphs_reverse_search(
        3, ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION);
    const auto legacy = enumerate_proper_interval_labeled_graphs_reverse_search(
        3, ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
    ASSERT_EQ(default_result.graphs.size(), 8u);
    ASSERT_EQ(dedicated.graphs.size(), 8u);
    ASSERT_EQ(legacy.graphs.size(), 8u);
    const std::vector<std::pair<int, int>> complete = {{1, 2}, {1, 3}, {2, 3}};
    EXPECT_EQ(default_result.graphs[0].edges, complete);
    const std::vector<std::vector<std::pair<int, int>>> legacy_order = {
        {}, {{2, 3}}, {{1, 3}}, {{1, 2}}, {{1, 2}, {2, 3}},
        {{1, 2}, {1, 3}}, {{1, 2}, {1, 3}, {2, 3}}, {{1, 3}, {2, 3}}
    };
    for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
        EXPECT_EQ(legacy.graphs[i].edges, legacy_order[i]);
    }
}

TEST(ProperIntervalLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {ProperIntervalLabeledEnumAlgorithm::STRING_REPRESENTATION,
                      ProperIntervalLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_proper_interval_labeled_graphs_reverse_search(-1, algo),
                     std::invalid_argument);
        EXPECT_THROW(enumerate_proper_interval_labeled_graphs_reverse_search(
                         graph_recognition::max_vertex_count + 1, algo),
                     std::invalid_argument);
    }
    EXPECT_THROW(enumerate_proper_interval_labeled_graphs_reverse_search(
                     0, static_cast<ProperIntervalLabeledEnumAlgorithm>(-1)),
                 std::invalid_argument);
}

}  // namespace
