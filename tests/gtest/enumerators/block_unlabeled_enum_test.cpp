#include "enumerators/block_labeled_enum.h"
#include "enumerators/block_unlabeled_enum.h"
#include "recognizers/block.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <set>
#include <utility>
#include <vector>

using namespace graph_recognition;
using graph_recognition::gtest_utils::canonical_edge_list;

namespace {

bool connected_graph(const Graph& g) {
    if (g.n == 0) return false;
    std::vector<int> stack(1, 1);
    std::vector<bool> seen(g.n + 1, false);
    seen[1] = true;
    for (std::size_t i = 0; i < stack.size(); ++i) {
        for (int v : g.adj[stack[i]]) {
            if (!seen[v]) {
                seen[v] = true;
                stack.push_back(v);
            }
        }
    }
    return stack.size() == static_cast<std::size_t>(g.n);
}

TEST(BlockUnlabeledEnumTest, KnownCountsValidityAndIsomorphismUniqueness) {
    const std::size_t connected_counts[] = {0, 1, 1, 2, 4, 9, 22, 59, 165, 496};
    const std::size_t all_counts[] = {1, 1, 2, 4, 9, 20, 50, 128, 351, 1009};
    for (int n = 0; n <= 9; ++n) {
        for (bool connected_only : {false, true}) {
            SCOPED_TRACE(::testing::Message() << n << " connected=" << connected_only);
            const auto result = enumerate_block_unlabeled_graphs(n, connected_only);
            EXPECT_EQ(result.graphs.size(), connected_only ? connected_counts[n] : all_counts[n]);
            std::set<std::vector<std::pair<int, int>>> classes;
            std::size_t connected_count = 0;
            for (const auto& graph : result.graphs) {
                EXPECT_EQ(graph.n, n);
                EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
                EXPECT_EQ(std::adjacent_find(graph.edges.begin(), graph.edges.end()), graph.edges.end());
                for (const auto& e : graph.edges) {
                    ASSERT_GE(e.first, 1);
                    ASSERT_LT(e.first, e.second);
                    ASSERT_LE(e.second, n);
                }
                const Graph g(n, graph.edges);
                EXPECT_TRUE(check_block(g).is_block);
                if (connected_graph(g)) ++connected_count;
                if (n <= 7) {
                    EXPECT_TRUE(classes.insert(canonical_edge_list(n, graph.edges)).second);
                }
            }
            EXPECT_EQ(connected_count, connected_counts[n]);
        }
    }
}

TEST(BlockUnlabeledEnumTest, MatchesLegacyLabeledIsomorphismClasses) {
    for (int n = 0; n <= 6; ++n) {
        SCOPED_TRACE(n);
        const auto legacy = enumerate_block_labeled_graphs_reverse_search(
            n, BlockLabeledEnumAlgorithm::REVERSE_SEARCH);
        const auto result = enumerate_block_unlabeled_graphs(n);
        std::set<std::vector<std::pair<int, int>>> expected, actual;
        for (const auto& graph : legacy.graphs)
            expected.insert(canonical_edge_list(n, graph.edges));
        for (const auto& graph : result.graphs)
            EXPECT_TRUE(actual.insert(canonical_edge_list(n, graph.edges)).second);
        EXPECT_EQ(actual, expected);
    }
}

TEST(BlockUnlabeledEnumTest, ZeroWeightBlocksAndCentroidSymmetries) {
    // Central K2 between equal halves; a zero-weight degree-3 block; two
    // adjacent centroids of different colors; and repeated clique branches.
    const std::vector<std::pair<int, std::vector<std::pair<int, int>>>> examples = {
        {4, {{1, 2}, {2, 3}, {3, 4}}},
        {6, {{1, 2}, {1, 3}, {2, 3}, {1, 4}, {2, 5}, {3, 6}}},
        {6, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}, {4, 5}, {5, 6}}},
        {7, {{1, 2}, {1, 3}, {2, 3}, {1, 4}, {1, 5}, {4, 5}, {1, 6}, {1, 7}, {6, 7}}}
    };
    for (const auto& example : examples) {
        const auto key = canonical_edge_list(example.first, example.second);
        const auto result = enumerate_block_unlabeled_graphs(example.first, true);
        int matches = 0;
        for (const auto& graph : result.graphs)
            if (canonical_edge_list(graph.n, graph.edges) == key) ++matches;
        EXPECT_EQ(matches, 1);
    }
}

TEST(BlockUnlabeledEnumTest, RejectsInvalidInputsAndAcceptsExplicitAlgorithm) {
    EXPECT_THROW(enumerate_block_unlabeled_graphs(-1), std::invalid_argument);
    EXPECT_THROW(enumerate_block_unlabeled_graphs(INT_MAX), std::invalid_argument);
    EXPECT_THROW(enumerate_block_unlabeled_graphs(
        0, false, static_cast<BlockUnlabeledEnumAlgorithm>(-1)), std::invalid_argument);
    EXPECT_EQ(enumerate_block_unlabeled_graphs(
        4, false, BlockUnlabeledEnumAlgorithm::BLOCK_GRAPH_GEN).graphs.size(), 9u);
}

}  // namespace
