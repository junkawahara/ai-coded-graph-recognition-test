#include "recognizers/five_leaf_power.h"
#include "recognizers/three_leaf_power.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::check_five_leaf_power;
using graph_recognition::check_three_leaf_power;

// The recognizer was rewritten around a 3-Steiner root search whose only
// enum-level coverage filters candidates through the recognizer itself.
// These invariants are external to that implementation:
//
//  - Every 3-leaf power is a 5-leaf power (subdivide each pendant edge of
//    a 3-leaf root once: all leaf distances grow by exactly 2). A
//    one-unit-too-aggressive prune in the Steiner search would fail this
//    for a family of genuine 5-leaf powers.
//  - Adding a true twin preserves 5-leaf-power membership in both
//    directions (duplicate the leaf one step above its pendant edge;
//    conversely an induced subgraph of a 5-leaf power is one).

std::vector<std::pair<int, int>> gen_clique_tree_substitution(int* n_out) {
    int t = 1 + std::rand() % 4;
    std::vector<int> parent(t, -1);
    for (int i = 1; i < t; ++i) parent[i] = std::rand() % i;
    std::vector<int> size(t), first(t);
    int n = 0;
    for (int i = 0; i < t; ++i) {
        size[i] = 1 + std::rand() % 3;
        first[i] = n + 1;
        n += size[i];
    }
    std::vector<std::pair<int, int>> edges;
    for (int i = 0; i < t; ++i) {
        for (int a = 0; a < size[i]; ++a)
            for (int b = a + 1; b < size[i]; ++b)
                edges.push_back(std::make_pair(first[i] + a, first[i] + b));
        if (parent[i] >= 0) {
            int p = parent[i];
            for (int a = 0; a < size[i]; ++a)
                for (int b = 0; b < size[p]; ++b)
                    edges.push_back(std::make_pair(first[i] + a, first[p] + b));
        }
    }
    *n_out = n;
    return edges;
}

TEST(FiveLeafPowerProperty, ThreeLeafPowersAreFiveLeafPowers) {
    std::srand(42);

    int three_leaf_hits = 0;
    for (int trial = 0; trial < 1500; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;
        if (trial % 2 == 0) {
            edges = gen_clique_tree_substitution(&n);
        } else {
            n = 1 + std::rand() % 8;
            double p = (std::rand() % 60) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        if (!check_three_leaf_power(g).is_three_leaf_power) continue;
        ++three_leaf_hits;
        EXPECT_TRUE(check_five_leaf_power(g).is_five_leaf_power)
            << "3-leaf power not accepted as 5-leaf power, trial=" << trial
            << " n=" << n << " m=" << edges.size();
    }
    // The clique-substitution generator must actually produce 3-leaf powers,
    // otherwise this test silently checks nothing.
    EXPECT_GT(three_leaf_hits, 300);
}

TEST(FiveLeafPowerProperty, TrueTwinAdditionPreservesMembership) {
    std::srand(43);

    for (int trial = 0; trial < 400; ++trial) {
        int n = 2 + std::rand() % 6;
        double p = 0.2 + (std::rand() % 60) / 100.0;
        std::vector<std::pair<int, int>> edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                if ((std::rand() % 1000) / 1000.0 < p)
                    edges.push_back(std::make_pair(u, v));

        Graph g(n, edges);
        bool before = check_five_leaf_power(g).is_five_leaf_power;

        // Duplicate a random vertex as a true twin (adjacent, same nbhd).
        int v = 1 + std::rand() % n;
        int tw = n + 1;
        std::vector<std::pair<int, int>> edges2 = edges;
        edges2.push_back(std::make_pair(v, tw));
        for (size_t i = 0; i < edges.size(); ++i) {
            if (edges[i].first == v)
                edges2.push_back(std::make_pair(tw, edges[i].second));
            else if (edges[i].second == v)
                edges2.push_back(std::make_pair(edges[i].first, tw));
        }
        Graph g2(n + 1, edges2);
        bool after = check_five_leaf_power(g2).is_five_leaf_power;

        ASSERT_EQ(before, after)
            << "true twin changed membership, trial=" << trial << " n=" << n
            << " v=" << v;
    }
}

}  // namespace
