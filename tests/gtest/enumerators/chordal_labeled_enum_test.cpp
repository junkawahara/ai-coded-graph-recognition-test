#include "recognizers/chordal.h"
#include "enumerators/chordal_labeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ChordalLabeledEnumAlgorithm;
using graph_recognition::ChordalLabeledEnumerationResult;
using graph_recognition::check_chordal;
using graph_recognition::enumerate_chordal_labeled_graphs_reverse_search;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "chordal_labeled_enum";

class ChordalLabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ChordalLabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ChordalLabeledEnumerationResult res = enumerate_chordal_labeled_graphs_reverse_search(n);
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
        EXPECT_TRUE(check_chordal(g).is_chordal) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ChordalLabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The streaming (callback) API must visit exactly the graphs the
// materializing API returns, in the same order.
TEST(ChordalLabeledEnumCallbackTest, StreamsSameGraphsAsMaterializingApi) {
    for (int n = 0; n <= 5; ++n) {
        ChordalLabeledEnumerationResult res = enumerate_chordal_labeled_graphs_reverse_search(n);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        graph_recognition::enumerate_chordal_labeled_graphs_reverse_search_cb(
            n, [&streamed](const graph_recognition::EnumeratedGraph& g) {
                streamed.push_back(g);
            });
        ASSERT_EQ(streamed.size(), res.graphs.size()) << "n=" << n;
        for (size_t i = 0; i < streamed.size(); ++i) {
            EXPECT_EQ(streamed[i].n, res.graphs[i].n) << "n=" << n << " #" << i;
            EXPECT_EQ(streamed[i].edges, res.graphs[i].edges) << "n=" << n << " #" << i;
        }
    }
}

TEST(ChordalLabeledEnumKiyomiUnoTest, MatchesOeisCountsThroughSixVertices) {
    const std::size_t expected[] = {1, 1, 2, 8, 61, 822, 18154};
    for (int n = 0; n <= 6; ++n) {
        ChordalLabeledEnumerationResult res = enumerate_chordal_labeled_graphs_reverse_search(
            n, ChordalLabeledEnumAlgorithm::KIYOMI_UNO);
        EXPECT_EQ(res.graphs.size(), expected[n]) << "n=" << n;
    }
}

TEST(ChordalLabeledEnumKiyomiUnoTest, EnumeratesSameSetsAsLegacySearch) {
    for (int n = 0; n <= 6; ++n) {
        ChordalLabeledEnumerationResult dedicated =
            enumerate_chordal_labeled_graphs_reverse_search(
                n, ChordalLabeledEnumAlgorithm::KIYOMI_UNO);
        ChordalLabeledEnumerationResult legacy =
            enumerate_chordal_labeled_graphs_reverse_search(
                n, ChordalLabeledEnumAlgorithm::LEGACY_VERTEX_REVERSE_SEARCH);

        std::set<std::vector<std::pair<int, int>>> dedicated_set;
        std::set<std::vector<std::pair<int, int>>> legacy_set;
        for (std::size_t i = 0; i < dedicated.graphs.size(); ++i) {
            EXPECT_TRUE(dedicated_set.insert(dedicated.graphs[i].edges).second)
                << "dedicated duplicate, n=" << n;
        }
        for (std::size_t i = 0; i < legacy.graphs.size(); ++i) {
            EXPECT_TRUE(legacy_set.insert(legacy.graphs[i].edges).second)
                << "legacy duplicate, n=" << n;
        }
        EXPECT_EQ(dedicated_set, legacy_set) << "n=" << n;
    }
}

TEST(ChordalLabeledEnumKiyomiUnoTest, ExplicitAlgorithmStreamsSameGraphs) {
    for (int n = 0; n <= 5; ++n) {
        ChordalLabeledEnumerationResult res = enumerate_chordal_labeled_graphs_reverse_search(
            n, ChordalLabeledEnumAlgorithm::KIYOMI_UNO);
        std::vector<graph_recognition::EnumeratedGraph> streamed;
        graph_recognition::enumerate_chordal_labeled_graphs_reverse_search_cb(
            n,
            [&streamed](const graph_recognition::EnumeratedGraph& g) {
                streamed.push_back(g);
            },
            ChordalLabeledEnumAlgorithm::KIYOMI_UNO);
        ASSERT_EQ(streamed.size(), res.graphs.size()) << "n=" << n;
        for (std::size_t i = 0; i < streamed.size(); ++i) {
            EXPECT_EQ(streamed[i].edges, res.graphs[i].edges)
                << "n=" << n << " #" << i;
        }
    }
}

}  // namespace
