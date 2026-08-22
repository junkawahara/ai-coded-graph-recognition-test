#include "split.h"
#include "split_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::SplitEnumAlgorithm;
using graph_recognition::SplitEnumerationResult;
using graph_recognition::check_split;
using graph_recognition::enumerate_split_graphs_reverse_search;
using graph_recognition::enumerate_split_graphs_reverse_search_cb;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "split_enum";

class SplitEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SplitEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    SplitEnumerationResult res = enumerate_split_graphs_reverse_search(n);
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
    AllCases, SplitEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

std::set<std::vector<std::pair<int, int>>> graph_set(
    const SplitEnumerationResult& result) {
    std::set<std::vector<std::pair<int, int>>> graphs;
    for (std::size_t i = 0; i < result.graphs.size(); ++i) {
        graphs.insert(result.graphs[i].edges);
    }
    return graphs;
}

TEST(SplitEnumCanonicalKSTest, EnumeratesSameSetsAsLegacyFilter) {
    for (int n = 0; n <= 6; ++n) {
        const SplitEnumerationResult dedicated =
            enumerate_split_graphs_reverse_search(
                n, SplitEnumAlgorithm::KS_PARTITION_CANONICAL);
        const SplitEnumerationResult legacy =
            enumerate_split_graphs_reverse_search(
                n, SplitEnumAlgorithm::LEGACY_CHORDAL_FILTER);

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

TEST(SplitEnumCanonicalKSTest, StreamsSameGraphsAsMaterializingApi) {
    for (int n = 0; n <= 5; ++n) {
        const SplitEnumerationResult materialized =
            enumerate_split_graphs_reverse_search(n);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        enumerate_split_graphs_reverse_search_cb(
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

TEST(SplitEnumCanonicalKSTest, StreamsKnownEightVertexCount) {
    std::size_t count = 0;
    enumerate_split_graphs_reverse_search_cb(
        8,
        [&count](const graph_recognition::EnumeratedGraph&) { ++count; });
    EXPECT_EQ(count, static_cast<std::size_t>(5843954));
}

TEST(SplitEnumCanonicalKSTest, NegativeOrderProducesNothing) {
    std::size_t count = 0;
    enumerate_split_graphs_reverse_search_cb(
        -1,
        [&count](const graph_recognition::EnumeratedGraph&) { ++count; });
    EXPECT_EQ(count, static_cast<std::size_t>(0));
}

}  // namespace
