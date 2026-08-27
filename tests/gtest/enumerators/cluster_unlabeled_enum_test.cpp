#include "recognizers/cluster.h"
#include "enumerators/cluster_labeled_enum.h"
#include "enumerators/cluster_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::ClusterUnlabeledEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_cluster;
using graph_recognition::enumerate_cluster_labeled_graphs;
using graph_recognition::enumerate_cluster_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::read_n;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "cluster_unlabeled_enum";

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

class ClusterUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(ClusterUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    int n = read_n(test_path(std::string(kDir) + "/" + stem + ".in"));
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    ClusterUnlabeledEnumerationResult res = enumerate_cluster_unlabeled_graphs(n);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > seen;
    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        std::vector<std::pair<int, int> > key = res.graphs[i].edges;
        for (size_t j = 0; j < key.size(); ++j) {
            if (key[j].first > key[j].second) std::swap(key[j].first, key[j].second);
        }
        // The construction is supposed to emit a sorted edge list already.
        EXPECT_TRUE(std::is_sorted(key.begin(), key.end()))
            << "unsorted edge list in case=" << stem;
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
        EXPECT_TRUE(check_cluster(g).is_cluster) << "invalid graph in case=" << stem;
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, ClusterUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// The connected cluster graphs are exactly the complete graphs, so
// connected_only must emit K_n and nothing else.
TEST(ClusterUnlabeledEnumConnectedTest, EmitsOnlyTheCompleteGraph) {
    for (int n = 1; n <= 8; ++n) {
        ClusterUnlabeledEnumerationResult res = enumerate_cluster_unlabeled_graphs(n, true);
        ASSERT_EQ(res.graphs.size(), 1u) << "n=" << n;
        EXPECT_EQ(res.graphs[0].n, n) << "n=" << n;
        EXPECT_EQ(static_cast<int>(res.graphs[0].edges.size()), n * (n - 1) / 2) << "n=" << n;
        EXPECT_EQ(component_count(res.graphs[0].n, res.graphs[0].edges), 1) << "n=" << n;

        Graph g(res.graphs[0].n, res.graphs[0].edges);
        EXPECT_TRUE(check_cluster(g).is_cluster) << "invalid graph for n=" << n;
    }
}

// Every component of an enumerated graph must be a clique, and the component
// sizes must come out as a distinct non-increasing partition of n.
TEST(ClusterUnlabeledEnumStructureTest, ComponentsAreCliquesInNonIncreasingOrder) {
    for (int n = 1; n <= 9; ++n) {
        ClusterUnlabeledEnumerationResult res = enumerate_cluster_unlabeled_graphs(n);
        std::set<std::vector<int> > partitions;
        for (size_t i = 0; i < res.graphs.size(); ++i) {
            Graph g(res.graphs[i].n, res.graphs[i].edges);
            std::vector<int> sizes;
            std::vector<bool> visited(n + 1, false);
            for (int v = 1; v <= n; ++v) {
                if (visited[v]) continue;
                std::vector<int> stack(1, v);
                std::vector<int> comp;
                visited[v] = true;
                while (!stack.empty()) {
                    int u = stack.back();
                    stack.pop_back();
                    comp.push_back(u);
                    for (size_t k = 0; k < g.adj[u].size(); ++k) {
                        int w = g.adj[u][k];
                        if (!visited[w]) {
                            visited[w] = true;
                            stack.push_back(w);
                        }
                    }
                }
                for (size_t k = 0; k < comp.size(); ++k) {
                    EXPECT_EQ(g.adj[comp[k]].size(), comp.size() - 1)
                        << "component is not a clique for n=" << n;
                }
                sizes.push_back(static_cast<int>(comp.size()));
            }
            int total = 0;
            for (size_t k = 0; k < sizes.size(); ++k) total += sizes[k];
            EXPECT_EQ(total, n) << "n=" << n;
            for (size_t k = 1; k < sizes.size(); ++k) {
                EXPECT_LE(sizes[k], sizes[k - 1]) << "parts not non-increasing for n=" << n;
            }
            EXPECT_TRUE(partitions.insert(sizes).second)
                << "duplicate partition for n=" << n;
        }
        EXPECT_EQ(partitions.size(), res.graphs.size()) << "n=" << n;
    }
}

// The unlabeled enumeration must cover exactly the isomorphism classes the
// labeled enumerator produces. Capped at n = 6 (203 labeled graphs x 720
// permutations).
TEST(ClusterUnlabeledEnumCrossCheckTest, MatchesLabeledEnumeratorClasses) {
    for (int n = 1; n <= 6; ++n) {
        std::set<std::vector<std::pair<int, int> > > labeled_classes;
        graph_recognition::ClusterLabeledEnumerationResult labeled =
            enumerate_cluster_labeled_graphs(n);
        for (size_t i = 0; i < labeled.graphs.size(); ++i) {
            labeled_classes.insert(
                canonical_edge_list(labeled.graphs[i].n, labeled.graphs[i].edges));
        }

        std::set<std::vector<std::pair<int, int> > > unlabeled_classes;
        ClusterUnlabeledEnumerationResult unlabeled = enumerate_cluster_unlabeled_graphs(n);
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
