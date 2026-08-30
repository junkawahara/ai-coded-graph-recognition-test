#include "recognizers/maximal_outer_planar.h"
#include "enumerators/maximal_outer_planar_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::MaximalOuterPlanarUnlabeledEnumerationResult;
using graph_recognition::check_maximal_outer_planar;
using graph_recognition::enumerate_maximal_outer_planar_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "maximal_outer_planar_unlabeled_enum";

class MaximalOuterPlanarUnlabeledEnumTest
    : public ::testing::TestWithParam<std::string> {};

TEST_P(MaximalOuterPlanarUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    MaximalOuterPlanarUnlabeledEnumerationResult res =
        enumerate_maximal_outer_planar_unlabeled_graphs(n);
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
        EXPECT_TRUE(check_maximal_outer_planar(g).is_maximal_outer_planar)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, MaximalOuterPlanarUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// There is no labeled maximal outerplanar enumerator to cross-check against,
// so brute-force one instead: all edge subsets of K_n of size 2n - 3 that the
// recognizer accepts, canonicalized, must be exactly the classes the
// enumerator produces. Capped at n = 6, the usual cross-check bound: n = 7
// (C(21, 11) = 352716 recognizer calls plus 7! canonicalizations per labeled
// hit) already takes about 10 s.
TEST(MaximalOuterPlanarUnlabeledEnumBruteForceTest, MatchesFilteredEdgeSubsets) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int> > all_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                all_edges.push_back(std::make_pair(u, v));
            }
        }
        const int mm = static_cast<int>(all_edges.size());
        const int target = n <= 1 ? 0 : 2 * n - 3;

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
            if (check_maximal_outer_planar(g).is_maximal_outer_planar) {
                brute_classes.insert(canonical_edge_list(n, edges));
            }
        }

        std::set<std::vector<std::pair<int, int> > > enum_classes;
        MaximalOuterPlanarUnlabeledEnumerationResult res =
            enumerate_maximal_outer_planar_unlabeled_graphs(n);
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

}  // namespace
