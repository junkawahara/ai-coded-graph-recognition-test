#include "recognizers/three_leaf_power.h"
#include "bf_oracles.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::check_three_leaf_power;
using graph_recognition::gtest_utils::bf_has_induced_cycle_ge;

// ---- Independent brute-force oracle -------------------------------------
// Characterization (Dom-Guo-Hueffner-Niedermeier 2006; Brandstaedt-Le):
// G is a 3-leaf power iff G is chordal and (bull, dart, gem)-free.
// Chordality and the forbidden induced subgraphs are checked by direct
// enumeration, sharing nothing with the library's critical-clique
// implementation.

// Does some 5-subset induce exactly the pattern (up to permutation)?
bool bf_has_induced_5pattern(int n, const std::vector<std::vector<bool>>& adj,
                             const bool pat[5][5]) {
    if (n < 5) return false;
    std::vector<int> vs(5);
    for (vs[0] = 1; vs[0] <= n; ++vs[0])
    for (vs[1] = vs[0] + 1; vs[1] <= n; ++vs[1])
    for (vs[2] = vs[1] + 1; vs[2] <= n; ++vs[2])
    for (vs[3] = vs[2] + 1; vs[3] <= n; ++vs[3])
    for (vs[4] = vs[3] + 1; vs[4] <= n; ++vs[4]) {
        int perm[5] = {0, 1, 2, 3, 4};
        do {
            bool match = true;
            for (int i = 0; i < 5 && match; ++i)
                for (int j = i + 1; j < 5 && match; ++j)
                    if (adj[vs[perm[i]]][vs[perm[j]]] != pat[i][j]) match = false;
            if (match) return true;
        } while (std::next_permutation(perm, perm + 5));
    }
    return false;
}

bool bf_is_three_leaf_power(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    if (bf_has_induced_cycle_ge(n, adj, 4)) return false;  // not chordal

    // bull: triangle 0,1,2 with pendants 3-0 and 4-1
    static const bool bull[5][5] = {
        {0, 1, 1, 1, 0},
        {1, 0, 1, 0, 1},
        {1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {0, 1, 0, 0, 0}};
    // dart: diamond 0,1,2,3 (missing edge 0-3) with pendant 4-1
    static const bool dart[5][5] = {
        {0, 1, 1, 0, 0},
        {1, 0, 1, 1, 1},
        {1, 1, 0, 1, 0},
        {0, 1, 1, 0, 0},
        {0, 1, 0, 0, 0}};
    // gem: path 0-1-2-3 plus universal vertex 4
    static const bool gem[5][5] = {
        {0, 1, 0, 0, 1},
        {1, 0, 1, 0, 1},
        {0, 1, 0, 1, 1},
        {0, 0, 1, 0, 1},
        {1, 1, 1, 1, 0}};

    if (bf_has_induced_5pattern(n, adj, bull)) return false;
    if (bf_has_induced_5pattern(n, adj, dart)) return false;
    if (bf_has_induced_5pattern(n, adj, gem)) return false;
    return true;
}

// Clique substitution into a random tree: guaranteed-YES-heavy generator.
std::vector<std::pair<int, int>> gen_clique_tree_substitution(int* n_out) {
    int t = 1 + std::rand() % 4;              // tree nodes
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

TEST(ThreeLeafPowerProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 2000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;
        if (trial % 2 == 0) {
            edges = gen_clique_tree_substitution(&n);
        } else {
            n = 1 + std::rand() % 8;
            double p = (std::rand() % 70) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);
        bool lib = check_three_leaf_power(g).is_three_leaf_power;
        bool bf = bf_is_three_leaf_power(n, edges);
        ASSERT_EQ(lib, bf)
            << "impl vs forbidden-subgraph oracle trial=" << trial
            << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
