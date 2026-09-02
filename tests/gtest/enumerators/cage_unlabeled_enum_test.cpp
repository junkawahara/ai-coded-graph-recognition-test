#include "enumerators/cage_unlabeled_enum.h"
#include "enumerators/kregular_unlabeled_enum.h"
#include "test_helpers.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <fstream>
#include <set>
#include <string>
#include <utility>
#include <vector>

using graph_recognition::CageUnlabeledEnumerationResult;
using graph_recognition::KRegularUnlabeledEnumerationResult;
using graph_recognition::cage_kg_graph_exists;
using graph_recognition::enumerate_cage_unlabeled_graphs;
using graph_recognition::enumerate_kregular_unlabeled_graphs;
using graph_recognition::gtest_utils::canonical_edge_list;
using graph_recognition::gtest_utils::list_in_files;
using graph_recognition::gtest_utils::read_expected;
using graph_recognition::gtest_utils::test_path;

namespace {

const char* kDir = "cage_unlabeled_enum";

// Girth straight from a second definition, independent of the search's
// pairwise-distance invariant: the shortest cycle through an edge (u, v)
// is 1 + the shortest u-v path avoiding that edge, so the girth is the
// minimum of that over all edges. Returns 0 when acyclic.
int brute_girth(int n, const std::vector<std::pair<int, int> >& edges) {
    std::vector<std::vector<int> > adj(n + 1);
    for (size_t e = 0; e < edges.size(); ++e) {
        adj[edges[e].first].push_back(edges[e].second);
        adj[edges[e].second].push_back(edges[e].first);
    }
    int girth = n + 1;
    for (size_t e = 0; e < edges.size(); ++e) {
        const int s = edges[e].first, t = edges[e].second;
        std::vector<int> dist(n + 1, -1);
        std::vector<int> queue;
        queue.push_back(s);
        dist[s] = 0;
        for (size_t head = 0; head < queue.size(); ++head) {
            const int v = queue[head];
            for (size_t j = 0; j < adj[v].size(); ++j) {
                const int u = adj[v][j];
                if (v == s && u == t) continue; /* skip the edge itself */
                if (dist[u] == -1) {
                    dist[u] = dist[v] + 1;
                    queue.push_back(u);
                }
            }
        }
        if (dist[t] != -1 && dist[t] + 1 < girth) girth = dist[t] + 1;
    }
    return girth > n ? 0 : girth;
}

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

// The number of partitions of n into parts >= g: what k = 2 must count
// (disjoint unions of cycles of length >= g).
int partitions_with_min_part(int n, int g) {
    // dp over largest-part-first choices: p(n, smallest allowed part)
    std::vector<std::vector<int> > memo(n + 1, std::vector<int>(n + 2, -1));
    struct Rec {
        std::vector<std::vector<int> >* memo;
        int operator()(int rest, int min_part) {
            if (rest == 0) return 1;
            if (min_part > rest) return 0;
            int& m = (*memo)[rest][min_part];
            if (m != -1) return m;
            int total = 0;
            for (int part = min_part; part <= rest; ++part) {
                total += (*this)(rest - part, part);
            }
            return m = total;
        }
    } rec = {&memo};
    return rec(n, g);
}

class CageUnlabeledEnumTest : public ::testing::TestWithParam<std::string> {};

TEST_P(CageUnlabeledEnumTest, CountAndAllValid) {
    const std::string stem = GetParam();
    std::ifstream fin(test_path(std::string(kDir) + "/" + stem + ".in").c_str());
    int n = 0, k = 0, g = 0;
    fin >> n >> k >> g;
    int expected = std::stoi(read_expected(test_path(std::string(kDir) + "/" + stem + ".exp")));

    CageUnlabeledEnumerationResult res =
        enumerate_cage_unlabeled_graphs(n, k, g);
    EXPECT_EQ(static_cast<int>(res.graphs.size()), expected) << "case=" << stem;

    std::set<std::vector<std::pair<int, int> > > canon_seen;
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(res.graphs[i].n, n) << "case=" << stem;

        // Every vertex must have degree exactly k, and the girth must be
        // >= g (0 = acyclic = infinite), checked against an independent
        // per-edge-BFS girth.
        std::vector<int> deg(n + 1, 0);
        for (size_t e = 0; e < res.graphs[i].edges.size(); ++e) {
            ++deg[res.graphs[i].edges[e].first];
            ++deg[res.graphs[i].edges[e].second];
        }
        for (int v = 1; v <= n; ++v) {
            EXPECT_EQ(deg[v], k) << "case=" << stem << " v=" << v;
        }
        const int girth = brute_girth(n, res.graphs[i].edges);
        EXPECT_TRUE(girth == 0 || girth >= g)
            << "girth " << girth << " < " << g << " in case=" << stem;

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
    }
}

INSTANTIATE_TEST_SUITE_P(
    AllCases, CageUnlabeledEnumTest,
    ::testing::ValuesIn(list_in_files(test_path(kDir))),
    [](const ::testing::TestParamInfo<std::string>& info) { return info.param; });

// g <= 3 places no constraint, so the enumeration must reproduce the
// k-regular one class-for-class.
TEST(CageUnlabeledEnumCrossCheckTest, MatchesKRegularAtGirth3) {
    for (int n = 1; n <= 8; ++n) {
        for (int k = 0; k <= 4; ++k) {
            std::set<std::vector<std::pair<int, int> > > mine;
            CageUnlabeledEnumerationResult res =
                enumerate_cage_unlabeled_graphs(n, k, 3);
            for (size_t i = 0; i < res.graphs.size(); ++i) {
                mine.insert(
                    canonical_edge_list(res.graphs[i].n, res.graphs[i].edges));
            }
            std::set<std::vector<std::pair<int, int> > > kreg;
            KRegularUnlabeledEnumerationResult kr =
                enumerate_kregular_unlabeled_graphs(n, k);
            for (size_t i = 0; i < kr.graphs.size(); ++i) {
                kreg.insert(
                    canonical_edge_list(kr.graphs[i].n, kr.graphs[i].edges));
            }
            EXPECT_EQ(mine, kreg) << "n=" << n << " k=" << k;
        }
    }
}

// Filtering the k-regular enumeration by the independent brute-force
// girth must yield exactly the (k,g)-enumeration, class for class.
TEST(CageUnlabeledEnumBruteForceTest, MatchesGirthFilteredKRegular) {
    for (int n = 3; n <= 8; ++n) {
        for (int k = 2; k <= 4; ++k) {
            for (int g = 4; g <= 6; ++g) {
                std::set<std::vector<std::pair<int, int> > > brute;
                KRegularUnlabeledEnumerationResult kr =
                    enumerate_kregular_unlabeled_graphs(n, k);
                for (size_t i = 0; i < kr.graphs.size(); ++i) {
                    const int girth = brute_girth(kr.graphs[i].n,
                                                  kr.graphs[i].edges);
                    if (girth == 0 || girth >= g) {
                        brute.insert(canonical_edge_list(kr.graphs[i].n,
                                                         kr.graphs[i].edges));
                    }
                }
                std::set<std::vector<std::pair<int, int> > > mine;
                CageUnlabeledEnumerationResult res =
                    enumerate_cage_unlabeled_graphs(n, k, g);
                for (size_t i = 0; i < res.graphs.size(); ++i) {
                    mine.insert(canonical_edge_list(res.graphs[i].n,
                                                    res.graphs[i].edges));
                }
                EXPECT_EQ(brute, mine)
                    << "n=" << n << " k=" << k << " g=" << g;
            }
        }
    }
}

// k = 2 must count the partitions of n into parts >= g, k = 1 the
// perfect matchings (one class, n even), k = 0 the edgeless graph.
TEST(CageUnlabeledEnumSmallDegreeTest, MatchesClosedForms) {
    for (int n = 1; n <= 12; ++n) {
        for (int g = 3; g <= 6; ++g) {
            CageUnlabeledEnumerationResult res =
                enumerate_cage_unlabeled_graphs(n, 2, g);
            EXPECT_EQ(static_cast<int>(res.graphs.size()),
                      n >= 3 ? partitions_with_min_part(n, g) : 0)
                << "k=2 n=" << n << " g=" << g;
        }
        // k <= 1 stops at n = 8: the (near-)edgeless graphs are the
        // exact canonicalization's factorial worst case.
        if (n <= 8) {
            EXPECT_EQ(enumerate_cage_unlabeled_graphs(n, 1, 5).graphs.size(),
                      n >= 2 && n % 2 == 0 ? 1u : 0u) << "k=1 n=" << n;
            EXPECT_EQ(enumerate_cage_unlabeled_graphs(n, 0, 5).graphs.size(),
                      1u) << "k=0 n=" << n;
        }
    }
}

// Connected mode: the Petersen graph is the only connected (3,5)-graph
// on 10 vertices, C_n the only connected (2,g)-graph for n >= g.
TEST(CageUnlabeledEnumConnectedTest, MatchesConnectedCounts) {
    CageUnlabeledEnumerationResult petersen =
        enumerate_cage_unlabeled_graphs(10, 3, 5, true);
    ASSERT_EQ(petersen.graphs.size(), 1u);
    EXPECT_EQ(component_count(10, petersen.graphs[0].edges), 1);
    EXPECT_EQ(brute_girth(10, petersen.graphs[0].edges), 5);

    EXPECT_EQ(enumerate_cage_unlabeled_graphs(10, 2, 5, true).graphs.size(),
              1u);
    EXPECT_EQ(enumerate_cage_unlabeled_graphs(12, 3, 5, true).graphs.size(),
              2u);
    CageUnlabeledEnumerationResult res =
        enumerate_cage_unlabeled_graphs(12, 3, 5, true);
    for (size_t i = 0; i < res.graphs.size(); ++i) {
        EXPECT_EQ(component_count(res.graphs[i].n, res.graphs[i].edges), 1);
    }
}

// The early-exit existence query must agree with emptiness of the full
// enumeration everywhere, including below the Moore bound.
TEST(CageUnlabeledEnumExistsTest, MatchesEnumerationEmptiness) {
    for (int n = 0; n <= 8; ++n) {
        for (int k = 0; k <= 4; ++k) {
            for (int g = 3; g <= 6; ++g) {
                EXPECT_EQ(cage_kg_graph_exists(n, k, g),
                          !enumerate_cage_unlabeled_graphs(n, k, g)
                               .graphs.empty())
                    << "n=" << n << " k=" << k << " g=" << g;
            }
        }
    }
    // Below the Moore bound M(3,7) = 22 nothing exists.
    EXPECT_FALSE(cage_kg_graph_exists(20, 3, 7));
    // The Petersen graph.
    EXPECT_TRUE(cage_kg_graph_exists(10, 3, 5));
}

TEST(CageUnlabeledEnumBoundaryTest, IntMinGirthIsUnconstrained) {
    CageUnlabeledEnumerationResult res =
        enumerate_cage_unlabeled_graphs(2, 0, INT_MIN);
    ASSERT_EQ(res.graphs.size(), 1u);
    EXPECT_TRUE(res.graphs[0].edges.empty());
    EXPECT_TRUE(cage_kg_graph_exists(2, 0, INT_MIN));
}

}  // namespace
