#include "recognizers/co_interval.h"
#include "enumerators/co_interval_labeled_enum.h"
#include "enum_property_helpers.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::CoIntervalLabeledEnumerationResult;
using graph_recognition::CoIntervalLabeledEnumAlgorithm;
using graph_recognition::check_co_interval;
using graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "co_interval_labeled_enum";

class CoIntervalLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CoIntervalLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    CoIntervalLabeledEnumerationResult res = enumerate_co_interval_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_co_interval(g).is_co_interval) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CoIntervalLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(CoIntervalLabeledEnumAlgorithmTest, MatchesLegacySetsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 61, 822, 17914};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_co_interval_labeled_graphs_reverse_search(
            n, CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO);
        const auto legacy = enumerate_co_interval_labeled_graphs_reverse_search(
            n, CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
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

TEST(CoIntervalLabeledEnumAlgorithmTest, MatchesBruteForceThroughSixVertices) {
    for (int n = 0; n <= 6; ++n) {
        const auto result = enumerate_co_interval_labeled_graphs_reverse_search(n);
        graph_recognition::gtest_utils::expect_enum_equals_bruteforce(
            n, [](const Graph& graph) { return check_co_interval(graph).is_co_interval; },
            result.graphs, "co-interval complement enumeration");
    }
}

TEST(CoIntervalLabeledEnumAlgorithmTest, DefaultsToComplementAndPreservesLegacyOrder) {
    const auto default_result = enumerate_co_interval_labeled_graphs_reverse_search(3);
    const auto dedicated = enumerate_co_interval_labeled_graphs_reverse_search(
        3, CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO);
    const auto legacy = enumerate_co_interval_labeled_graphs_reverse_search(
        3, CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
    ASSERT_EQ(default_result.graphs.size(), 8u);
    ASSERT_EQ(dedicated.graphs.size(), 8u);
    ASSERT_EQ(legacy.graphs.size(), 8u);
    // KKU starts at K3 and then deletes (1, 2), which becomes a complement edge.
    EXPECT_TRUE(default_result.graphs[0].edges.empty());
    const std::vector<std::pair<int, int>> first_edge = {{1, 2}};
    EXPECT_EQ(default_result.graphs[1].edges, first_edge);
    const std::vector<std::vector<std::pair<int, int>>> legacy_order = {
        {}, {{1, 3}}, {{2, 3}}, {{1, 3}, {2, 3}},
        {{1, 2}}, {{1, 2}, {1, 3}}, {{1, 2}, {2, 3}},
        {{1, 2}, {1, 3}, {2, 3}}
    };
    for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
        EXPECT_EQ(legacy.graphs[i].edges, legacy_order[i]);
    }
}

TEST(CoIntervalLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {CoIntervalLabeledEnumAlgorithm::KIYOMI_KIJIMA_UNO,
                      CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_co_interval_labeled_graphs_reverse_search(-1, algo),
                     std::invalid_argument);
        EXPECT_THROW(enumerate_co_interval_labeled_graphs_reverse_search(65, algo),
                     std::invalid_argument);
    }
    EXPECT_THROW(enumerate_co_interval_labeled_graphs_reverse_search(
                     0, static_cast<CoIntervalLabeledEnumAlgorithm>(-1)),
                 std::invalid_argument);
}

}  // namespace
