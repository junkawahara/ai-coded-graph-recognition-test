#include "recognizers/apollonian.h"
#include "recognizers/chordal.h"
#include "enumerators/apollonian_unlabeled_enum.h"
#include "enumerators/maximal_planar_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::ApollonianUnlabeledEnumerationResult;
using graph_recognition::MaximalPlanarUnlabeledEnumerationResult;
using graph_recognition::check_apollonian;
using graph_recognition::check_chordal;
using graph_recognition::enumerate_apollonian_unlabeled_graphs;
using graph_recognition::enumerate_maximal_planar_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "apollonian_unlabeled_enum";

class ApollonianUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(ApollonianUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ApollonianUnlabeledEnumerationResult res =
        enumerate_apollonian_unlabeled_graphs(n);
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
        // permutation; guarded to n <= 7 since it costs n! per graph).
        if (res.graphs[i].n <= 7) {
            EXPECT_TRUE(canon_seen
                            .insert(canonical_edge_list(res.graphs[i].n,
                                                        res.graphs[i].edges))
                            .second)
                << "isomorphic duplicate in case=" << stem;
        }

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_apollonian(g).is_apollonian)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ApollonianUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// There is no labeled Apollonian enumerator to cross-check against, so
// brute-force one instead: all edge subsets of K_n of size 3n - 6 that the
// recognizer accepts, canonicalized, must be exactly the classes the
// enumerator produces. Capped at n = 6 (C(15, 12) = 455 subsets); the usual
// cross-check bound.
TEST(ApollonianUnlabeledEnumBruteForceTest, MatchesFilteredEdgeSubsets) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int> > all_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                all_edges.push_back(std::make_pair(u, v));
            }
        }
        const int mm = static_cast<int>(all_edges.size());
        const int target = n <= 2 ? (n == 2 ? 1 : 0) : 3 * n - 6;

        std::set<std::vector<std::pair<int, int> > > brute_classes;
        for (unsigned long long mask = 0; mask < (1ULL << mm); ++mask) {
#if defined(__GNUC__)
            if (__builtin_popcountll(mask) != target) continue;
#endif
            std::vector<std::pair<int, int> > edges;
            for (int b = 0; b < mm; ++b) {
                if ((mask >> b) & 1ULL) edges.push_back(all_edges[b]);
            }
            if (static_cast<int>(edges.size()) != target) continue;
            Graph g(n, edges);
            if (check_apollonian(g).is_apollonian) {
                brute_classes.insert(canonical_edge_list(n, edges));
            }
        }

        std::set<std::vector<std::pair<int, int> > > enum_classes;
        ApollonianUnlabeledEnumerationResult res =
            enumerate_apollonian_unlabeled_graphs(n);
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            enum_classes.insert(
                canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
        }

        EXPECT_EQ(res.graphs.size(), enum_classes.size())
            << "isomorphic duplicates for n=" << n;
        EXPECT_EQ(enum_classes, brute_classes)
            << "class set differs from the brute-force filter for n=" << n;
    }
}

// Independent route above the brute-force cap: Apollonian networks are
// exactly the chordal maximal planar graphs, so filtering the maximal planar
// unlabeled enumerator by chordality must give the same class sets. Runs to
// n = 8 (14 maximal planar classes, of which 7 are chordal; n = 9 would
// spend seconds in the n! brute-force canonical_edge_list).
TEST(ApollonianUnlabeledEnumCrossCheckTest, MatchesChordalMaximalPlanar) {
    for (int n = 4; n <= 8; ++n) {
        std::set<std::vector<std::pair<int, int> > > mp_classes;
        MaximalPlanarUnlabeledEnumerationResult mp =
            enumerate_maximal_planar_unlabeled_graphs(n);
        for (size_t i = 0; i < mp.graphs.size(); ++i) {
            Graph g(mp.graphs[i].n, mp.graphs[i].edges);
            if (check_chordal(g).is_chordal) {
                mp_classes.insert(
                    canonical_edge_list(mp.graphs[i].n, mp.graphs[i].edges));
            }
        }

        std::set<std::vector<std::pair<int, int> > > enum_classes;
        ApollonianUnlabeledEnumerationResult res =
            enumerate_apollonian_unlabeled_graphs(n);
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            enum_classes.insert(
                canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
        }

        EXPECT_EQ(enum_classes, mp_classes)
            << "class set differs from chordal-filtered maximal planar for n="
            << n;
    }
}

}  // namespace
