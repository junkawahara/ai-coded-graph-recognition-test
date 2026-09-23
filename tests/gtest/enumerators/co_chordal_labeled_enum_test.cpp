#include "recognizers/co_chordal.h"
#include "enumerators/co_chordal_labeled_enum.h"
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
using graph_recognition::CoChordalLabeledEnumerationResult;
using graph_recognition::CoChordalLabeledEnumAlgorithm;
using graph_recognition::check_co_chordal;
using graph_recognition::enumerate_co_chordal_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "co_chordal_labeled_enum";

class CoChordalLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CoChordalLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    CoChordalLabeledEnumerationResult res = enumerate_co_chordal_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_co_chordal(g).is_co_chordal) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CoChordalLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(CoChordalLabeledEnumAlgorithmTest, MatchesLegacySetsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 61, 822, 18154};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_co_chordal_labeled_graphs_reverse_search(
            n, CoChordalLabeledEnumAlgorithm::KIYOMI_UNO);
        const auto legacy = enumerate_co_chordal_labeled_graphs_reverse_search(
            n, CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH);
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

TEST(CoChordalLabeledEnumAlgorithmTest, MatchesBruteForceThroughSixVertices) {
    for (int n = 0; n <= 6; ++n) {
        const auto result = enumerate_co_chordal_labeled_graphs_reverse_search(n);
        graph_recognition::gtest_utils::expect_enum_equals_bruteforce(
            n, [](const Graph& graph) { return check_co_chordal(graph).is_co_chordal; },
            result.graphs, "co-chordal complement enumeration");
    }
}

TEST(CoChordalLabeledEnumAlgorithmTest, DefaultsToComplementAndPreservesLegacyOrder) {
    // The chordal search starts at the edgeless graph, whose complement is K2.
    const auto default_result = enumerate_co_chordal_labeled_graphs_reverse_search(2);
    const auto dedicated = enumerate_co_chordal_labeled_graphs_reverse_search(
        2, CoChordalLabeledEnumAlgorithm::KIYOMI_UNO);
    const auto legacy = enumerate_co_chordal_labeled_graphs_reverse_search(
        2, CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH);
    ASSERT_EQ(default_result.graphs.size(), 2u);
    ASSERT_EQ(dedicated.graphs.size(), 2u);
    ASSERT_EQ(legacy.graphs.size(), 2u);
    const std::vector<std::pair<int, int>> complete = {{1, 2}};
    EXPECT_EQ(default_result.graphs[0].edges, complete);
    EXPECT_TRUE(default_result.graphs[1].edges.empty());
    for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
    }
    EXPECT_TRUE(legacy.graphs[0].edges.empty());
    EXPECT_EQ(legacy.graphs[1].edges, complete);
}

TEST(CoChordalLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {CoChordalLabeledEnumAlgorithm::KIYOMI_UNO,
                      CoChordalLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_co_chordal_labeled_graphs_reverse_search(-1, algo),
                     std::invalid_argument);
        EXPECT_THROW(enumerate_co_chordal_labeled_graphs_reverse_search(65, algo),
                     std::invalid_argument);
    }
    EXPECT_THROW(enumerate_co_chordal_labeled_graphs_reverse_search(
                     0, static_cast<CoChordalLabeledEnumAlgorithm>(-1)),
                 std::invalid_argument);
}

}  // namespace
