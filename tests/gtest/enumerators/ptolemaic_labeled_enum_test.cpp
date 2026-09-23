#include "recognizers/ptolemaic.h"
#include "enumerators/ptolemaic_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::PtolemaicLabeledEnumerationResult;
using graph_recognition::PtolemaicLabeledEnumAlgorithm;
using graph_recognition::check_ptolemaic;
using graph_recognition::enumerate_ptolemaic_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "ptolemaic_labeled_enum";

class PtolemaicLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PtolemaicLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    PtolemaicLabeledEnumerationResult res = enumerate_ptolemaic_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_ptolemaic(g).is_ptolemaic) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PtolemaicLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(PtolemaicLabeledEnumAlgorithmTest, MatchesLegacySetsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 61, 762, 13534};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_ptolemaic_labeled_graphs_reverse_search(
            n, PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL);
        const auto legacy = enumerate_ptolemaic_labeled_graphs_reverse_search(
            n, PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH);
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
        EXPECT_EQ(dedicated_set.count({}), 1u);
        if (n == 4) {
            // False twins with a clique neighborhood form a diamond; false
            // twins with a non-clique neighborhood would create an induced C4.
            EXPECT_EQ(dedicated_set.count({{1, 2}, {1, 3}, {2, 3}, {2, 4}, {3, 4}}), 1u);
            EXPECT_EQ(dedicated_set.count({{1, 2}, {1, 4}, {2, 3}, {3, 4}}), 0u);
        }
        if (n == 5) {
            // Vertex 5 of this P5 is neither pendant nor a twin; extensions
            // restricted to increasing labels would miss this labeling.
            EXPECT_EQ(dedicated_set.count({{1, 2}, {2, 5}, {3, 4}, {3, 5}}), 1u);
        }
        if (n == 6) {
            // Repeated components, with interleaved labels and with isolates.
            EXPECT_EQ(dedicated_set.count({{1, 3}, {1, 5}, {2, 4}, {2, 6}, {3, 5}, {4, 6}}), 1u);
            EXPECT_EQ(dedicated_set.count({{1, 6}, {2, 4}}), 1u);
        }
    }
}

TEST(PtolemaicLabeledEnumAlgorithmTest, DefaultsToVertexIncrementalAndPreservesLegacyOrder) {
    const auto default_result = enumerate_ptolemaic_labeled_graphs_reverse_search(3);
    const auto dedicated = enumerate_ptolemaic_labeled_graphs_reverse_search(
        3, PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL);
    const auto legacy = enumerate_ptolemaic_labeled_graphs_reverse_search(
        3, PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH);
    ASSERT_EQ(default_result.graphs.size(), 8u);
    ASSERT_EQ(dedicated.graphs.size(), 8u);
    ASSERT_EQ(legacy.graphs.size(), 8u);
    EXPECT_FALSE(default_result.graphs[0].edges.empty());
    const std::vector<std::vector<std::pair<int, int>>> legacy_order = {
        {}, {{2, 3}}, {{1, 3}}, {{1, 2}}, {{1, 2}, {2, 3}},
        {{1, 2}, {1, 3}}, {{1, 2}, {1, 3}, {2, 3}}, {{1, 3}, {2, 3}}
    };
    for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
        EXPECT_EQ(legacy.graphs[i].edges, legacy_order[i]);
    }
}

TEST(PtolemaicLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL,
                      PtolemaicLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_ptolemaic_labeled_graphs_reverse_search(-1, algo),
                     std::invalid_argument);
        EXPECT_THROW(enumerate_ptolemaic_labeled_graphs_reverse_search(INT_MAX, algo),
                     std::invalid_argument);
    }
    EXPECT_THROW(enumerate_ptolemaic_labeled_graphs_reverse_search(64),
                 std::invalid_argument);
    EXPECT_THROW(enumerate_ptolemaic_labeled_graphs_reverse_search(
                     64, PtolemaicLabeledEnumAlgorithm::VERTEX_INCREMENTAL),
                 std::invalid_argument);
    EXPECT_THROW(enumerate_ptolemaic_labeled_graphs_reverse_search(
                     0, static_cast<PtolemaicLabeledEnumAlgorithm>(-1)),
                 std::invalid_argument);
}

}  // namespace
