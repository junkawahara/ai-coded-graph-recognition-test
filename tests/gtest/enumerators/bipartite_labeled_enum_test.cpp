#include "recognizers/bipartite.h"
#include "enumerators/bipartite_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::BipartiteLabeledEnumAlgorithm;
using graph_recognition::BipartiteLabeledEnumerationResult;
using graph_recognition::check_bipartite;
using graph_recognition::enumerate_bipartite_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "bipartite_labeled_enum";

TEST(BipartiteLabeledEnumAlgorithmTest, MatchesLegacyGraphSetsWithoutDuplicates) {
    const std::size_t counts[] = {1, 1, 2, 7, 41, 376, 5177};
    for (int n = 0; n <= 6; ++n) {
        const auto dedicated = enumerate_bipartite_labeled_graphs_reverse_search(
            n, BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING);
        const auto legacy = enumerate_bipartite_labeled_graphs_reverse_search(
            n, BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH);
        const auto defaults = enumerate_bipartite_labeled_graphs_reverse_search(n);
        ASSERT_EQ(dedicated.graphs.size(), counts[n]) << n;
        ASSERT_EQ(legacy.graphs.size(), counts[n]) << n;
        ASSERT_EQ(defaults.graphs.size(), counts[n]) << n;
        std::set<std::vector<std::pair<int, int>>> actual, expected;
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            const auto& graph = dedicated.graphs[i];
            EXPECT_EQ(graph.n, n);
            EXPECT_EQ(defaults.graphs[i].edges, graph.edges);
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            for (const auto& edge : graph.edges) {
                EXPECT_GE(edge.first, 1);
                EXPECT_LT(edge.first, edge.second);
                EXPECT_LE(edge.second, n);
            }
            EXPECT_TRUE(actual.insert(graph.edges).second);
            EXPECT_TRUE(expected.insert(legacy.graphs[i].edges).second);
        }
        EXPECT_EQ(actual, expected) << n;
    }
}

TEST(BipartiteLabeledEnumAlgorithmTest, PreservesLegacyOrder) {
    const auto result = enumerate_bipartite_labeled_graphs_reverse_search(
        3, BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH);
    const std::vector<std::vector<std::pair<int, int>>> expected = {
        {}, {{1, 3}}, {{2, 3}}, {{1, 3}, {2, 3}},
        {{1, 2}}, {{1, 2}, {1, 3}}, {{1, 2}, {2, 3}}};
    ASSERT_EQ(result.graphs.size(), expected.size());
    for (std::size_t i = 0; i < expected.size(); ++i)
        EXPECT_EQ(result.graphs[i].edges, expected[i]);
}

TEST(BipartiteLabeledEnumAlgorithmTest, RejectsInvalidInputsForBothAlgorithms) {
    for (auto algo : {BipartiteLabeledEnumAlgorithm::BIPARTITE_COLORING,
                      BipartiteLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        for (int n : {-1, 65})
            EXPECT_THROW(enumerate_bipartite_labeled_graphs_reverse_search(n, algo),
                         std::invalid_argument);
    }
    for (int n : {0, 2})
        EXPECT_THROW(enumerate_bipartite_labeled_graphs_reverse_search(
            n, static_cast<BipartiteLabeledEnumAlgorithm>(999)), std::invalid_argument);
}

class BipartiteLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(BipartiteLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    BipartiteLabeledEnumerationResult res = enumerate_bipartite_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_bipartite(g).is_bipartite) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, BipartiteLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
