#include "recognizers/bipartite_permutation.h"
#include "enumerators/bipartite_permutation_labeled_enum.h"
#include "enumerators/bipartite_permutation_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::BipartitePermutationUnlabeledEnumerationResult;
using graph_recognition::check_bipartite_permutation;
using graph_recognition::enumerate_bipartite_permutation_labeled_graphs_reverse_search;
using graph_recognition::enumerate_bipartite_permutation_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "bipartite_permutation_unlabeled_enum";

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

class BipartitePermutationUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(BipartitePermutationUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    BipartitePermutationUnlabeledEnumerationResult res =
        enumerate_bipartite_permutation_unlabeled_graphs(n);
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
        // permutation; guarded to n <= 7, since n = 8 would be 239 graphs
        // times 8! orderings).
        if (res.graphs[i].n <= 7) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_bipartite_permutation(g).is_bipartite_permutation)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, BipartitePermutationUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// connected_only must reproduce the connected bipartite permutation counts
// (1, 1, 1, 3, 5, 16, 38 for n = 1..7; the sequence is not in the OEIS),
// with every graph connected, valid and pairwise non-isomorphic.
TEST(BipartitePermutationUnlabeledEnumConnectedTest, MatchesConnectedCounts) {
    const int expected[] = {1, 1, 1, 3, 5, 16, 38};
    for (int n = 1; n <= 7; ++n) {
        BipartitePermutationUnlabeledEnumerationResult res =
            enumerate_bipartite_permutation_unlabeled_graphs(n, true);
        EXPECT_EQ(static_cast<int>(res.graphs.size()), expected[n - 1]) << "n=" << n;

        std::set<std::vector<std::pair<int, int> > > canon_seen;
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            EXPECT_EQ(res.graphs[i].n, n) << "n=" << n;
            EXPECT_EQ(component_count(res.graphs[i].n, res.graphs[i].edges), 1)
                << "disconnected graph for n=" << n;
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate for n=" << n;

            Graph g(res.graphs[i].n, res.graphs[i].edges);
            EXPECT_TRUE(check_bipartite_permutation(g).is_bipartite_permutation)
                << "invalid graph for n=" << n;
        }
    }
}

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (canonicalization is 6! per
// labeled graph, and the labeled counts grow like 2^(n log n)).
TEST(BipartitePermutationUnlabeledEnumCrossCheckTest,
     MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::BipartitePermutationLabeledEnumerationResult labeled =
            enumerate_bipartite_permutation_labeled_graphs_reverse_search(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        BipartitePermutationUnlabeledEnumerationResult unlabeled =
            enumerate_bipartite_permutation_unlabeled_graphs(n);
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
