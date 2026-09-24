#include "recognizers/block.h"
#include "enumerators/block_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::BlockLabeledEnumerationResult;
using graph_recognition::BlockLabeledEnumAlgorithm;
using graph_recognition::check_block;
using graph_recognition::enumerate_block_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "block_labeled_enum";

class BlockLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(BlockLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    BlockLabeledEnumerationResult res = enumerate_block_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_block(g).is_block) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, BlockLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

TEST(BlockLabeledEnumAlgorithmTest, MatchesLegacyAndExhaustiveRecognitionThroughSixVertices) {
    const std::size_t counts[] = {1, 1, 2, 8, 55, 562, 7739};
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto dedicated = enumerate_block_labeled_graphs_reverse_search(
            n, BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN);
        const auto legacy = enumerate_block_labeled_graphs_reverse_search(
            n, BlockLabeledEnumAlgorithm::REVERSE_SEARCH);
        std::set<std::vector<std::pair<int, int>>> actual, previous, exhaustive;
        EXPECT_EQ(dedicated.graphs.size(), counts[n]);
        for (const auto& graph : dedicated.graphs) {
            EXPECT_EQ(graph.n, n);
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            for (const auto& edge : graph.edges) {
                EXPECT_GE(edge.first, 1);
                EXPECT_LT(edge.first, edge.second);
                EXPECT_LE(edge.second, n);
            }
            EXPECT_TRUE(actual.insert(graph.edges).second);
        }
        for (const auto& graph : legacy.graphs) EXPECT_TRUE(previous.insert(graph.edges).second);
        // An independent oracle over all simple graphs, with no enumeration parent rule.
        const int m = n * (n - 1) / 2;
        for (unsigned mask = 0; mask < (1u << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            int bit = 0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v, ++bit)
                    if (mask & (1u << bit)) edges.emplace_back(u, v);
            if (check_block(Graph(n, edges)).is_block) exhaustive.insert(edges);
        }
        EXPECT_EQ(actual, previous);
        EXPECT_EQ(actual, exhaustive);
        if (n == 5) {
            // The largest label is an articulation, not a removable simplicial vertex.
            EXPECT_EQ(actual.count({{1, 5}, {2, 5}, {3, 5}, {4, 5}}), 1u);
        }
        if (n == 6) {
            // Repeated components with interleaved labels, plus isolates.
            EXPECT_EQ(actual.count({{1, 3}, {1, 5}, {2, 4}, {2, 6}, {3, 5}, {4, 6}}), 1u);
            EXPECT_EQ(actual.count({{1, 6}, {2, 4}}), 1u);
        }
    }
}

TEST(BlockLabeledEnumAlgorithmTest, DefaultAndLegacyOrder) {
    const auto default_result = enumerate_block_labeled_graphs_reverse_search(3);
    const auto dedicated = enumerate_block_labeled_graphs_reverse_search(
        3, BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN);
    const auto legacy = enumerate_block_labeled_graphs_reverse_search(
        3, BlockLabeledEnumAlgorithm::REVERSE_SEARCH);
    const std::vector<std::vector<std::pair<int, int>>> legacy_order = {
        {}, {{2, 3}}, {{1, 3}}, {{1, 2}}, {{1, 2}, {2, 3}},
        {{1, 2}, {1, 3}}, {{1, 2}, {1, 3}, {2, 3}}, {{1, 3}, {2, 3}}
    };
    ASSERT_EQ(default_result.graphs.size(), 8u);
    ASSERT_EQ(dedicated.graphs.size(), 8u);
    ASSERT_EQ(legacy.graphs.size(), 8u);
    EXPECT_FALSE(default_result.graphs[0].edges.empty());
    for (std::size_t i = 0; i < legacy_order.size(); ++i) {
        EXPECT_EQ(default_result.graphs[i].edges, dedicated.graphs[i].edges);
        EXPECT_EQ(legacy.graphs[i].edges, legacy_order[i]);
    }
}

TEST(BlockLabeledEnumAlgorithmTest, RejectsInvalidInputs) {
    for (auto algo : {BlockLabeledEnumAlgorithm::BLOCK_GRAPH_GEN,
                      BlockLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        EXPECT_THROW(enumerate_block_labeled_graphs_reverse_search(-1, algo), std::invalid_argument);
        EXPECT_THROW(enumerate_block_labeled_graphs_reverse_search(INT_MAX, algo), std::invalid_argument);
    }
    EXPECT_THROW(enumerate_block_labeled_graphs_reverse_search(
        0, static_cast<BlockLabeledEnumAlgorithm>(-1)), std::invalid_argument);
}

}  // namespace
