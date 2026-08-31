#include "recognizers/degenerate.h"
#include "enumerators/degenerate_unlabeled_enum.h"
#include "enumerators/forest_unlabeled_enum.h"
#include "enumerators/partial_ktree_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::DegenerateUnlabeledEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_degenerate;
using graph_recognition::enumerate_degenerate_unlabeled_graphs;
using graph_recognition::enumerate_forest_unlabeled_graphs;
using graph_recognition::enumerate_partial_ktree_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "degenerate_unlabeled_enum";

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

class DegenerateUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(DegenerateUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    std::ifstream fin(test_path(std::string(kDir) + "/" + stem + ".in").c_str());
    int n = 0, k = 0;
    fin >> n >> k;
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    DegenerateUnlabeledEnumerationResult res =
        enumerate_degenerate_unlabeled_graphs(n, k);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(res.graphs[i].n, n) << "case=" << stem;

        // One representative per isomorphism class: reject isomorphic
        // duplicates (canonical form by brute-force permutation; guarded
        // to n <= 7 to keep the n! cost negligible).
        if (res.graphs[i].n <= 7) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_degenerate(g, k).is_degenerate)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, DegenerateUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// Brute-force cross-check: canonicalizing every edge subset of K_n that
// passes the recognizer must yield exactly the enumerator's classes.
TEST(DegenerateUnlabeledEnumBruteForceTest, MatchesFilteredEdgeSubsets) {
    for (int n = 1; n <= 5; ++n) {
        std::vector<std::pair<int, int> > pairs;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                pairs.push_back(std::make_pair(u, v));
            }
        }
        for (int k = 0; k <= 3; ++k) {
            std::set<std::vector<std::pair<int, int> > > brute;
            for (int mask = 0; mask < (1 << (int)pairs.size()); ++mask) {
                std::vector<std::pair<int, int> > edges;
                for (size_t e = 0; e < pairs.size(); ++e) {
                    if (mask & (1 << (int)e)) edges.push_back(pairs[e]);
                }
                Graph g(n, edges);
                if (check_degenerate(g, k).is_degenerate) {
                    brute.insert(canonical_edge_list(n, edges));
                }
            }

            std::set<std::vector<std::pair<int, int> > > enumerated;
            DegenerateUnlabeledEnumerationResult res =
                enumerate_degenerate_unlabeled_graphs(n, k);
            for (size_t i = 0; i < res.graphs.size(); ++i) {
                enumerated.insert(
                    canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
            }
            EXPECT_EQ(brute, enumerated) << "n=" << n << " k=" << k;
        }
    }
}

// k = 1 must reproduce the unlabeled forests class-for-class, and the
// partial k-tree classes must be a subset (treewidth <= k implies
// k-degenerate), proper from n = 5, k = 2 on.
TEST(DegenerateUnlabeledEnumCrossCheckTest, MatchesForestsAndContainsPartialKTrees) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > mine;
        DegenerateUnlabeledEnumerationResult res =
            enumerate_degenerate_unlabeled_graphs(n, 1);
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
    }
    for (int n = 1; n <= 5; ++n) {
        for (int k = 1; k <= 3; ++k) {
            std::set<std::vector<std::pair<int, int> > > mine;
            DegenerateUnlabeledEnumerationResult res =
                enumerate_degenerate_unlabeled_graphs(n, k);
            for (size_t i = 0; i < res.graphs.size(); ++i) {
                mine.insert(
                    canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
            }
            graph_recognition::PartialKTreeUnlabeledEnumerationResult pr =
                enumerate_partial_ktree_unlabeled_graphs(n, k);
            EXPECT_GE(mine.size(), pr.graphs.size()) << "n=" << n << " k=" << k;
            for (size_t i = 0; i < pr.graphs.size(); ++i) {
                EXPECT_TRUE(mine.count(
                    canonical_edge_list(pr.graphs[i].n, pr.graphs[i].edges)))
                    << "partial k-tree class missing: n=" << n << " k=" << k;
            }
            if (n == 5 && k == 2) {
                EXPECT_EQ(mine.size(), pr.graphs.size() + 1)
                    << "K4 with one edge subdivided should be the only extra";
            }
        }
    }
}

// connected_only for k = 1 must yield the trees (A000055), with every
// graph connected and pairwise non-isomorphic.
TEST(DegenerateUnlabeledEnumConnectedTest, MatchesConnectedCounts) {
    const int tree_counts[] = {1, 1, 1, 2, 3, 6, 11};  // A000055, n = 1..7
    for (int n = 1; n <= 7; ++n) {
        DegenerateUnlabeledEnumerationResult res =
            enumerate_degenerate_unlabeled_graphs(n, 1, true);
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
    // Connected 2-degenerate counts (computed by this enumerator and
    // pinned here; not in the OEIS).
    const int conn2_counts[] = {1, 1, 2, 5, 16, 68};  // n = 1..6
    for (int n = 1; n <= 6; ++n) {
        DegenerateUnlabeledEnumerationResult res =
            enumerate_degenerate_unlabeled_graphs(n, 2, true);
        EXPECT_EQ(static_cast<int>(res.graphs.size()), conn2_counts[n - 1])
            << "n=" << n;
    }
}

}  // namespace
