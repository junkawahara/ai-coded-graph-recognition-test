#include "recognizers/triangle_free.h"
#include "enumerators/triangle_free_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::TriangleFreeLabeledEnumAlgorithm;
using graph_recognition::TriangleFreeLabeledEnumerationResult;
using graph_recognition::check_triangle_free;
using graph_recognition::enumerate_triangle_free_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "triangle_free_labeled_enum";

TEST(TriangleFreeLabeledEnumAlgorithmTest, MatchesLegacyWithoutDuplicatesInTheSameOrder) {
    const std::size_t counts[] = {1, 1, 2, 7, 41, 388, 5789};
    for (int n = 0; n <= 6; ++n) {
        const auto dedicated = enumerate_triangle_free_labeled_graphs_reverse_search(
            n, TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS);
        const auto legacy = enumerate_triangle_free_labeled_graphs_reverse_search(
            n, TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH);
        const auto defaults = enumerate_triangle_free_labeled_graphs_reverse_search(n);
        ASSERT_EQ(dedicated.graphs.size(), counts[n]) << n;
        ASSERT_EQ(legacy.graphs.size(), counts[n]) << n;
        ASSERT_EQ(defaults.graphs.size(), counts[n]) << n;
        std::set<std::vector<std::pair<int, int>>> seen;
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            const auto& graph = dedicated.graphs[i];
            EXPECT_EQ(graph.n, n);
            EXPECT_EQ(defaults.graphs[i].edges, graph.edges);
            EXPECT_EQ(legacy.graphs[i].edges, graph.edges);
            EXPECT_TRUE(std::is_sorted(graph.edges.begin(), graph.edges.end()));
            for (const auto& edge : graph.edges) {
                EXPECT_GE(edge.first, 1);
                EXPECT_LT(edge.first, edge.second);
                EXPECT_LE(edge.second, n);
            }
            EXPECT_TRUE(seen.insert(graph.edges).second);
        }
    }
}

TEST(TriangleFreeLabeledEnumAlgorithmTest, RejectsInvalidInputsForBothAlgorithms) {
    for (auto algo : {TriangleFreeLabeledEnumAlgorithm::INDEPENDENT_SETS,
                      TriangleFreeLabeledEnumAlgorithm::REVERSE_SEARCH}) {
        for (int n : {-1, 65})
            EXPECT_THROW(enumerate_triangle_free_labeled_graphs_reverse_search(n, algo),
                         std::invalid_argument);
    }
    for (int n : {0, 2})
        EXPECT_THROW(enumerate_triangle_free_labeled_graphs_reverse_search(
            n, static_cast<TriangleFreeLabeledEnumAlgorithm>(999)), std::invalid_argument);
}

class TriangleFreeLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(TriangleFreeLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    TriangleFreeLabeledEnumerationResult res = enumerate_triangle_free_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_triangle_free(g).is_triangle_free) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, TriangleFreeLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
