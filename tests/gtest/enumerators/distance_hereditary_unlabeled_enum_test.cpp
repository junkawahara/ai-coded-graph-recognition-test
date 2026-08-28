#include "recognizers/distance_hereditary.h"
#include "enumerators/distance_hereditary_labeled_enum.h"
#include "enumerators/distance_hereditary_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::DistanceHereditaryUnlabeledEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_distance_hereditary;
using graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search;
using graph_recognition::enumerate_distance_hereditary_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "distance_hereditary_unlabeled_enum";

// canonical_edge_list is brute force over n! permutations, so the
// isomorphic-duplicate checks below stop above this size (n = 8 would be
// 2078 graphs x 40320 permutations).
const int kCanonMaxN = 7;

// Counts the connected components of an enumerated graph by union-find.
int component_count(int n, const std::vector<std::pair<int, int> >& edges) {
    if (n <= 0) return 0;
    std::vector<int> parent(n + 1);
    for (int v = 1; v <= n; ++v) parent[v] = v;
    int components = n;
    for (size_t e = 0; e < edges.size(); ++e) {
        int a = edges[e].first;
        while (parent[a] != a) a = parent[a];
        int b = edges[e].second;
        while (parent[b] != b) b = parent[b];
        if (a != b) {
            parent[a] = b;
            --components;
        }
    }
    return components;
}

class DistanceHereditaryUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(DistanceHereditaryUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    DistanceHereditaryUnlabeledEnumerationResult res =
        enumerate_distance_hereditary_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        // This class enumerates one representative per isomorphism class, so
        // also reject isomorphic duplicates (canonical form by brute-force
        // permutation).
        if (res.graphs[i].n <= kCanonMaxN) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_distance_hereditary(g).is_distance_hereditary)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DistanceHereditaryUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// connected_only must reproduce A277862 (connected non-isomorphic
// distance-hereditary graphs) with every graph connected, valid and pairwise
// non-isomorphic.
TEST(DistanceHereditaryUnlabeledEnumConnectedTest, MatchesA277862) {
    const int expected[] = {1, 1, 2, 6, 18, 73, 308, 1484};
    for (int n = 1; n <= 8; ++n) {
        DistanceHereditaryUnlabeledEnumerationResult res =
            enumerate_distance_hereditary_unlabeled_graphs(n, true);
        EXPECT_EQ(static_cast<int>(res.graphs.size()), expected[n - 1]) << "n=" << n;

        std::set<std::vector<std::pair<int, int> > > canon_seen;
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            EXPECT_EQ(res.graphs[i].n, n) << "n=" << n;
            EXPECT_EQ(component_count(res.graphs[i].n, res.graphs[i].edges), 1)
                << "disconnected graph for n=" << n;
            if (n <= kCanonMaxN) {
                EXPECT_TRUE(canon_seen
                                .insert(canonical_edge_list(res.graphs[i].n,
                                                            res.graphs[i].edges))
                                .second)
                    << "isomorphic duplicate for n=" << n;
            }

            Graph g(res.graphs[i].n, res.graphs[i].edges);
            EXPECT_TRUE(check_distance_hereditary(g).is_distance_hereditary)
                << "invalid graph for n=" << n;
        }
    }
}

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (18344 labeled graphs x 720
// permutations); n = 7 would be 498416 x 5040.
TEST(DistanceHereditaryUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::DistanceHereditaryLabeledEnumerationResult labeled =
            enumerate_distance_hereditary_labeled_graphs_reverse_search(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        DistanceHereditaryUnlabeledEnumerationResult unlabeled =
            enumerate_distance_hereditary_unlabeled_graphs(n);
        for (size_t i = 0; i < unlabeled.graphs.size(); ++i) {
            unlabeled_classes.insert(
                canonical_edge_list(unlabeled.graphs[i].n, unlabeled.graphs[i].edges));
        }

        EXPECT_EQ(unlabeled.graphs.size(), unlabeled_classes.size())
            << "isomorphic duplicates for n=" << n;
        EXPECT_EQ(unlabeled_classes, labeled_classes)
            << "class set differs from the labeled enumerator for n=" << n;
    }
}

}  // namespace
