#include "recognizers/partial_ktree.h"
#include "enumerators/forest_unlabeled_enum.h"
#include "enumerators/partial_ktree_unlabeled_enum.h"
#include "enumerators/series_parallel_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::PartialKTreeUnlabeledEnumerationResult;
using graph_recognition::check_partial_ktree;
using graph_recognition::enumerate_forest_unlabeled_graphs;
using graph_recognition::enumerate_partial_ktree_unlabeled_graphs;
using graph_recognition::enumerate_series_parallel_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "partial_ktree_unlabeled_enum";

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

class PartialKTreeUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(PartialKTreeUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    std::ifstream fin(test_path(std::string(kDir) + "/" + stem + ".in").c_str());
    int n = 0, k = 0;
    fin >> n >> k;
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    PartialKTreeUnlabeledEnumerationResult res =
        enumerate_partial_ktree_unlabeled_graphs(n, k);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(res.graphs[i].n, n) << "case=" << stem;

        // One representative per isomorphism class: reject isomorphic
        // duplicates (canonical form by brute-force permutation; guarded
        // to n <= 8 to keep the n! cost negligible).
        if (res.graphs[i].n <= 8) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_partial_ktree(g, k).is_partial_ktree)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, PartialKTreeUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Brute-force cross-check: canonicalizing every edge subset of K_n that
// passes the recognizer must yield exactly the enumerator's classes.
TEST(PartialKTreeUnlabeledEnumBruteForceTest, MatchesFilteredEdgeSubsets) {
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int> > pairs;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                pairs.push_back(std::make_pair(u, v));
            }
        }
        for (int k = 1; k <= 3; ++k) {
            std::set<std::vector<std::pair<int, int> > > brute;
            for (int mask = 0; mask < (1 << (int)pairs.size()); ++mask) {
                std::vector<std::pair<int, int> > edges;
                for (size_t e = 0; e < pairs.size(); ++e) {
                    if (mask & (1 << (int)e)) edges.push_back(pairs[e]);
                }
                Graph g(n, edges);
                if (check_partial_ktree(g, k).is_partial_ktree) {
                    brute.insert(canonical_edge_list(n, edges));
                }
            }

            std::set<std::vector<std::pair<int, int> > > enumerated;
            PartialKTreeUnlabeledEnumerationResult res =
                enumerate_partial_ktree_unlabeled_graphs(n, k);
            for (size_t i = 0; i < res.graphs.size(); ++i) {
                enumerated.insert(
                    canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
            }
            EXPECT_EQ(brute, enumerated) << "n=" << n << " k=" << k;
        }
    }
}

// k = 1 must reproduce the unlabeled forests and k = 2 the unlabeled
// series-parallel (K4-minor-free) graphs, class for class.
TEST(PartialKTreeUnlabeledEnumCrossCheckTest, MatchesForestsAndSeriesParallel) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > mine;
        PartialKTreeUnlabeledEnumerationResult res =
            enumerate_partial_ktree_unlabeled_graphs(n, 1);
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            mine.insert(
                canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
        }
        std::set<std::vector<std::pair<int, int> > > forests;
        graph_recognition::ForestUnlabeledEnumerationResult fr =
            enumerate_forest_unlabeled_graphs(n);
        for (size_t i = 0; i < fr.graphs.size(); ++i) {
            forests.insert(
                canonical_edge_list(fr.graphs[i].n, fr.graphs[i].edges));
        }
        EXPECT_EQ(mine, forests) << "k=1 n=" << n;

        mine.clear();
        res = enumerate_partial_ktree_unlabeled_graphs(n, 2);
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            mine.insert(
                canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
        }
        std::set<std::vector<std::pair<int, int> > > sp;
        graph_recognition::SeriesParallelUnlabeledEnumerationResult sr =
            enumerate_series_parallel_unlabeled_graphs(n);
        for (size_t i = 0; i < sr.graphs.size(); ++i) {
            sp.insert(canonical_edge_list(sr.graphs[i].n, sr.graphs[i].edges));
        }
        EXPECT_EQ(mine, sp) << "k=2 n=" << n;
    }
}

// connected_only for k = 1 must yield the trees (A000055), and for k = 2
// must match the series-parallel enumerator's connected mode, with every
// graph connected and pairwise non-isomorphic.
TEST(PartialKTreeUnlabeledEnumConnectedTest, MatchesConnectedCounts) {
    const int tree_counts[] = {1, 1, 1, 2, 3, 6, 11};  // A000055, n = 1..7
    for (int n = 1; n <= 7; ++n) {
        PartialKTreeUnlabeledEnumerationResult res =
            enumerate_partial_ktree_unlabeled_graphs(n, 1, true);
        EXPECT_EQ(static_cast<int>(res.graphs.size()), tree_counts[n - 1])
            << "n=" << n;
        std::set<std::vector<std::pair<int, int> > > canon_seen;
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            EXPECT_EQ(component_count(res.graphs[i].n, res.graphs[i].edges), 1)
                << "n=" << n;
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate at n=" << n;
        }
    }
    for (int n = 1; n <= 6; ++n) {
        PartialKTreeUnlabeledEnumerationResult res =
            enumerate_partial_ktree_unlabeled_graphs(n, 2, true);
        graph_recognition::SeriesParallelUnlabeledEnumerationResult sr =
            enumerate_series_parallel_unlabeled_graphs(n, true);
        EXPECT_EQ(res.graphs.size(), sr.graphs.size()) << "n=" << n;
    }
}

TEST(PartialKTreeUnlabeledEnumBoundaryTest, IntMaxEnumeratesAllGraphs) {
    PartialKTreeUnlabeledEnumerationResult res =
        enumerate_partial_ktree_unlabeled_graphs(3, INT_MAX);
    EXPECT_EQ(res.graphs.size(), 4u);  // A000088(3)
}

}  // namespace
