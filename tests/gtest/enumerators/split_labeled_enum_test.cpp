#include "split.h"
#include "split_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::SplitLabeledEnumAlgorithm;
using graph_recognition::SplitLabeledEnumerationResult;
using graph_recognition::check_split;
using graph_recognition::enumerate_split_labeled_graphs_reverse_search;
using graph_recognition::enumerate_split_labeled_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "split_labeled_enum";

class SplitLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SplitLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    SplitLabeledEnumerationResult res = enumerate_split_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_split(g).is_split) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SplitLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

std::set<std::vector<std::pair<int, int>>> graph_set(
    const SplitLabeledEnumerationResult& result) {
    std::set<std::vector<std::pair<int, int>>> graphs;
    for (std::size_t i = 0; i < result.graphs.size(); ++i) {
        graphs.insert(result.graphs[i].edges);
    }
    return graphs;
}

TEST(SplitLabeledEnumCanonicalKSTest, EnumeratesSameSetsAsLegacyFilter) {
    for (int n = 0; n <= 6; ++n) {
        const SplitLabeledEnumerationResult dedicated =
            enumerate_split_labeled_graphs_reverse_search(
                n, SplitLabeledEnumAlgorithm::KS_PARTITION_CANONICAL);
        const SplitLabeledEnumerationResult legacy =
            enumerate_split_labeled_graphs_reverse_search(
                n, SplitLabeledEnumAlgorithm::LEGACY_CHORDAL_FILTER);

        const std::set<std::vector<std::pair<int, int>>> dedicated_set =
            graph_set(dedicated);
        const std::set<std::vector<std::pair<int, int>>> legacy_set =
            graph_set(legacy);
        EXPECT_EQ(dedicated_set.size(), dedicated.graphs.size())
            << "dedicated duplicate, n=" << n;
        EXPECT_EQ(legacy_set.size(), legacy.graphs.size())
            << "legacy duplicate, n=" << n;
        EXPECT_EQ(dedicated_set, legacy_set) << "n=" << n;
    }
}

TEST(SplitLabeledEnumCanonicalKSTest, StreamsSameGraphsAsMaterializingApi) {
    for (int n = 0; n <= 5; ++n) {
        const SplitLabeledEnumerationResult materialized =
            enumerate_split_labeled_graphs_reverse_search(n);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        enumerate_split_labeled_graphs_reverse_search_cb(
            n,
            [&streamed](const graph_recognition::EnumeratedGraph& graph) {
                streamed.push_back(graph);
            });

        ASSERT_EQ(streamed.size(), materialized.graphs.size()) << "n=" << n;
        for (std::size_t i = 0; i < streamed.size(); ++i) {
            EXPECT_EQ(streamed[i].n, materialized.graphs[i].n)
                << "n=" << n << " #" << i;
            EXPECT_EQ(streamed[i].edges, materialized.graphs[i].edges)
                << "n=" << n << " #" << i;
        }
    }
}

TEST(SplitLabeledEnumCanonicalKSTest, StreamsKnownEightVertexCount) {
    std::size_t count = 0;
    enumerate_split_labeled_graphs_reverse_search_cb(
        8,
        [&count](const graph_recognition::EnumeratedGraph&) { ++count; });
    EXPECT_EQ(count, static_cast<std::size_t>(5843954));
}

TEST(SplitLabeledEnumCanonicalKSTest, NegativeOrderProducesNothing) {
    std::size_t count = 0;
    enumerate_split_labeled_graphs_reverse_search_cb(
        -1,
        [&count](const graph_recognition::EnumeratedGraph&) { ++count; });
    EXPECT_EQ(count, static_cast<std::size_t>(0));
}

}  // namespace
