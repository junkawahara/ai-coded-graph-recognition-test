#include "recognizers/snark.h"
#include "enumerators/snark_labeled_enum.h"
#include "enumerators/snark_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::Graph;
using graph_recognition::SnarkUnlabeledEnumerationResult;
using graph_recognition::check_snark;
using graph_recognition::enumerate_snark_labeled_graphs;
using graph_recognition::enumerate_snark_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "snark_unlabeled_enum";

class SnarkUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(SnarkUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    SnarkUnlabeledEnumerationResult res = enumerate_snark_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    // Every level is either empty or a single graph within the test range
    // (the Petersen graph at n = 10), so labeled duplicates cannot occur and
    // the brute-force n! canonicalization (unusable from n = 10 anyway) is
    // not needed; validity via the recognizer is the meaningful check.
    std::set<std::vector<std::pair<int, int> > > seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        std::sort(key.begin(), key.end());
        EXPECT_TRUE(seen.insert(key).second) << "duplicate graph in case=" << stem;

        Graph g(res.graphs[i].n, res.graphs[i].edges);
        EXPECT_TRUE(check_snark(g).is_snark)
            << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, SnarkUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Within the n <= 6 brute-force
// canonicalization cap both sides are empty (the smallest snark has 10
// vertices), so this only pins down the shared empty levels; the n = 10
// level is covered by the A130315 count plus recognizer validity above
// (the Petersen graph is the unique snark on 10 vertices).
TEST(SnarkUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::SnarkLabeledEnumerationResult labeled =
            enumerate_snark_labeled_graphs(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        SnarkUnlabeledEnumerationResult unlabeled =
            enumerate_snark_unlabeled_graphs(n);
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

// The single n = 10 output must be the Petersen graph: 3-regular with
// girth 5 on 10 vertices forces it (the (3,5)-cage is unique), but check
// the distance-regularity fingerprint directly anyway: diameter 2 and
// exactly 6 vertices at distance 2 from each vertex.
TEST(SnarkUnlabeledEnumPetersenTest, N10IsPetersen) {
    SnarkUnlabeledEnumerationResult res = enumerate_snark_unlabeled_graphs(10);
    ASSERT_EQ(res.graphs.size(), 1u);
    const int n = res.graphs[0].n;
    ASSERT_EQ(n, 10);
    Graph g(n, res.graphs[0].edges);
    for (int s = 1; s <= n; ++s) {
        std::vector<int> dist(n + 1, -1);
        std::vector<int> bfs(1, s);
        dist[s] = 0;
        for (size_t qi = 0; qi < bfs.size(); ++qi) {
            int v = bfs[qi];
            for (size_t i = 0; i < g.adj[v].size(); ++i) {
                int u = g.adj[v][i];
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    bfs.push_back(u);
                }
            }
        }
        int at1 = 0, at2 = 0;
        for (int v = 1; v <= n; ++v) {
            ASSERT_NE(dist[v], -1);
            EXPECT_LE(dist[v], 2);
            if (dist[v] == 1) ++at1;
            if (dist[v] == 2) ++at2;
        }
        EXPECT_EQ(at1, 3);
        EXPECT_EQ(at2, 6);
    }
}

}  // namespace
