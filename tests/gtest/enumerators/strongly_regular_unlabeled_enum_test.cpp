#include "recognizers/strongly_regular.h"
#include "enumerators/strongly_regular_labeled_enum.h"
#include "enumerators/strongly_regular_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::StronglyRegularLabeledEnumerationResult;
using graph_recognition::StronglyRegularUnlabeledEnumerationResult;
using graph_recognition::check_strongly_regular;
using graph_recognition::enumerate_strongly_regular_labeled_graphs;
using graph_recognition::enumerate_strongly_regular_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "strongly_regular_unlabeled_enum";

class StronglyRegularUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(StronglyRegularUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(
        read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    StronglyRegularUnlabeledEnumerationResult res =
        enumerate_strongly_regular_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(res.graphs[i].n, n) << "case=" << stem;
        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
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
        EXPECT_TRUE(check_strongly_regular(g).is_strongly_regular)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, StronglyRegularUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces, within the brute-force canonicalization cap
// (n = 7 is included because the labeled output there is empty and cheap).
TEST(StronglyRegularUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 7; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        StronglyRegularLabeledEnumerationResult labeled =
            enumerate_strongly_regular_labeled_graphs(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(canonical_edge_list(labeled.graphs[i].n,
                                                       labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        StronglyRegularUnlabeledEnumerationResult unlabeled =
            enumerate_strongly_regular_unlabeled_graphs(n);
        for (size_t i = 0; i < unlabeled.graphs.size(); ++i) {
            unlabeled_classes.insert(canonical_edge_list(
                unlabeled.graphs[i].n, unlabeled.graphs[i].edges));
        }

        EXPECT_EQ(unlabeled.graphs.size(), unlabeled_classes.size())
            << "isomorphic duplicates for n=" << n;
        EXPECT_EQ(unlabeled_classes, labeled_classes)
            << "class set differs from the labeled enumerator for n=" << n;
    }
}

// A strongly regular graph determines its parameters (k, lambda, mu)
// uniquely, so the per-parameter searches must partition the output: the
// multiset of parameter tuples read off the emitted graphs has no graph
// under two tuples, and every tuple's degree matches the graph.
TEST(StronglyRegularUnlabeledEnumParameterTest, ParametersReadBackConsistently) {
    for (int n = 4; n <= 10; ++n) {
        StronglyRegularUnlabeledEnumerationResult res =
            enumerate_strongly_regular_unlabeled_graphs(n);
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            std::vector<int> deg(n + 1, 0);
            for (size_t j = 0; j < res.graphs[i].edges.size(); ++j) {
                ++deg[res.graphs[i].edges[j].first];
                ++deg[res.graphs[i].edges[j].second];
            }
            for (int v = 2; v <= n; ++v) {
                EXPECT_EQ(deg[v], deg[1]) << "n=" << n << " graph " << i;
            }
            EXPECT_GT(deg[1], 0) << "n=" << n << " graph " << i;
            EXPECT_LT(deg[1], n - 1) << "n=" << n << " graph " << i;
        }
    }
}

}  // namespace
