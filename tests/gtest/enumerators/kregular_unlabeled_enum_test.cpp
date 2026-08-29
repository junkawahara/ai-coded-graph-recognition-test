#include "recognizers/kregular.h"
#include "enumerators/cubic_unlabeled_enum.h"
#include "enumerators/kregular_labeled_enum.h"
#include "enumerators/kregular_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::KRegularUnlabeledEnumerationResult;
using graph_recognition::check_kregular;
using graph_recognition::enumerate_cubic_unlabeled_graphs;
using graph_recognition::enumerate_kregular_labeled_graphs_reverse_search;
using graph_recognition::enumerate_kregular_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "kregular_unlabeled_enum";

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

class KRegularUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(KRegularUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    std::ifstream fin(test_path(std::string(kDir) + "/" + stem + ".in").c_str());
    int n = 0, k = 0;
    fin >> n >> k;
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    KRegularUnlabeledEnumerationResult res =
        enumerate_kregular_unlabeled_graphs(n, k);
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
        // permutation; guarded to n <= 8 to keep the n! cost negligible).
        if (res.graphs[i].n <= 8) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        graph_recognition::KRegularResult r = check_kregular(g);
        EXPECT_TRUE(r.is_kregular) << "invalid graph in case=" << stem;
        EXPECT_EQ(r.k, k) << "wrong k in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, KRegularUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// connected_only for k = 2 must yield exactly the single cycle, and for
// k = 4 must reproduce A006820 (connected 4-regular graphs), with every
// graph connected, k-regular and pairwise non-isomorphic.
TEST(KRegularUnlabeledEnumConnectedTest, MatchesConnectedCounts) {
    struct CaseSpec {
        int n;
        int k;
        int expected;
    };
    const CaseSpec cases[] = {
        {3, 2, 1}, {6, 2, 1}, {9, 2, 1},           // single cycle
        {5, 4, 1}, {6, 4, 1}, {7, 4, 2}, {8, 4, 6},
        {9, 4, 16}, {10, 4, 59},                    // A006820
    };
    for (size_t c = 0; c < sizeof(cases) / sizeof(cases[0]); ++c) {
        const int n = cases[c].n;
        const int k = cases[c].k;
        KRegularUnlabeledEnumerationResult res =
            enumerate_kregular_unlabeled_graphs(n, k, true);
        EXPECT_EQ(static_cast<int>(res.graphs.size()), cases[c].expected)
            << "n=" << n << " k=" << k;

        std::set<std::vector<std::pair<int, int> > > canon_seen;
        for (size_t j = 0; j < res.graphs.size(); ++j) {
            EXPECT_EQ(res.graphs[j].n, n) << "n=" << n << " k=" << k;
            EXPECT_EQ(component_count(res.graphs[j].n, res.graphs[j].edges), 1)
                << "disconnected graph for n=" << n << " k=" << k;
            if (n <= 8) {
                EXPECT_TRUE(canon_seen
                                .insert(canonical_edge_list(res.graphs[j].n,
                                                            res.graphs[j].edges))
                                .second)
                    << "isomorphic duplicate for n=" << n << " k=" << k;
            }

            Graph g(res.graphs[j].n, res.graphs[j].edges);
            graph_recognition::KRegularResult r = check_kregular(g);
            EXPECT_TRUE(r.is_kregular) << "invalid graph for n=" << n << " k=" << k;
            EXPECT_EQ(r.k, k) << "wrong k for n=" << n << " k=" << k;
        }
    }
}

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces, for every degree 0 <= k < n within the
// n <= 6 brute-force canonicalization cap.
TEST(KRegularUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        for (int k = 0; k < n; ++k) {
            std::set<std::vector<std::pair<int, int> > > labeled_classes;
            graph_recognition::KRegularLabeledEnumerationResult labeled =
                enumerate_kregular_labeled_graphs_reverse_search(n, k);
            for (size_t i = 0; i < labeled.graphs.size(); ++i) {
                labeled_classes.insert(canonical_edge_list(
                    labeled.graphs[i].n, labeled.graphs[i].edges));
            }

            std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
            KRegularUnlabeledEnumerationResult unlabeled =
                enumerate_kregular_unlabeled_graphs(n, k);
            for (size_t i = 0; i < unlabeled.graphs.size(); ++i) {
                unlabeled_classes.insert(canonical_edge_list(
                    unlabeled.graphs[i].n, unlabeled.graphs[i].edges));
            }

            EXPECT_EQ(unlabeled.graphs.size(), unlabeled_classes.size())
                << "isomorphic duplicates for n=" << n << " k=" << k;
            EXPECT_EQ(unlabeled_classes, labeled_classes)
                << "class set differs from the labeled enumerator for n=" << n
                << " k=" << k;
        }
    }
}

// k = 3 must agree with the dedicated cubic unlabeled enumerator's counts.
TEST(KRegularUnlabeledEnumCubicTest, MatchesCubicUnlabeledCounts) {
    for (int n = 4; n <= 10; ++n) {
        graph_recognition::CubicUnlabeledEnumerationResult cubic =
            enumerate_cubic_unlabeled_graphs(n);
        KRegularUnlabeledEnumerationResult res =
            enumerate_kregular_unlabeled_graphs(n, 3);
        EXPECT_EQ(res.graphs.size(), cubic.graphs.size()) << "n=" << n;
    }
}

}  // namespace
