#include "recognizers/threshold.h"
#include "enumerators/threshold_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ThresholdUnlabeledEnumerationResult;
using graph_recognition::check_threshold;
using graph_recognition::enumerate_threshold_unlabeled_graphs;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;
using graph_recognition::gtest_utils::canonical_edge_list;

namespace {

const char* kDir = "threshold_unlabeled_enum";

class ThresholdUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ThresholdUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ThresholdUnlabeledEnumerationResult res = enumerate_threshold_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int>>> seen;
    std::set<std::vector<std::pair<int, int>>> canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int>> key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation; guarded to n <= 8 to keep the n! cost negligible).
        if (res.graphs[i].n <= 8) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_threshold(g).is_threshold) << "invalid graph in case=" << stem;
    }
}

// The streaming entry point exists so that a caller (the CLI above all) never
// has to hold 2^(n-1) graphs at once: it must emit exactly the same sequence.
TEST(ThresholdUnlabeledEnumTest, StreamingMatchesTheMaterializedResult) {
    for (int n = 0; n <= 12; ++n) {
        std::vector<std::pair<int, std::vector<std::pair<int, int> > > > streamed;
        graph_recognition::enumerate_threshold_unlabeled_graphs_cb(
            n, [&streamed](const graph_recognition::
                               ThresholdUnlabeledEnumeratedGraph& g) {
                streamed.push_back(std::make_pair(g.n, g.edges));
            });

        ThresholdUnlabeledEnumerationResult res =
            enumerate_threshold_unlabeled_graphs(n);
        ASSERT_EQ(streamed.size(), res.graphs.size()) << "n=" << n;
        if (n >= 1) {
            EXPECT_EQ(streamed.size(), (size_t)1u << (n - 1)) << "n=" << n;
        }
        for (size_t i = 0; i < streamed.size(); ++i) {
            EXPECT_EQ(streamed[i].first, res.graphs[i].n) << "n=" << n << " #" << i;
            EXPECT_EQ(streamed[i].second, res.graphs[i].edges)
                << "n=" << n << " #" << i;
        }
    }
}

// Out of the bitmask's range in both entry points: the documented empty
// result, not a length_error from reserving 2^63 graphs.
TEST(ThresholdUnlabeledEnumTest, OutOfRangeVertexCountsYieldNothing) {
    const int counts[3] = {-1, 64, 1000};
    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(enumerate_threshold_unlabeled_graphs(counts[i]).graphs.empty())
            << "n=" << counts[i];
        int streamed = 0;
        graph_recognition::enumerate_threshold_unlabeled_graphs_cb(
            counts[i],
            [&streamed](const graph_recognition::
                            ThresholdUnlabeledEnumeratedGraph&) { ++streamed; });
        EXPECT_EQ(streamed, 0) << "n=" << counts[i];
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ThresholdUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

}  // namespace
