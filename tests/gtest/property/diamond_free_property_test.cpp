#include "diamond_free.h"
#include "certificates.h"
#include "claw_free.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::DiamondFreeAlgorithm;
using graph_recognition::DiamondFreeResult;
using graph_recognition::check_diamond_free;
using graph_recognition::gtest_utils::verify_obstruction;

// Check if graph is a forest (acyclic) via DFS
bool is_forest(const Graph& g) {
    int n = g.n;
    std::vector<int> visited(n + 1, 0);
    std::vector<int> stack;
    std::vector<int> parent(n + 1, 0);

    for (int s = 1; s <= n; ++s) {
        if (visited[s]) continue;
        stack.push_back(s);
        visited[s] = 1;
        parent[s] = -1;
        while (!stack.empty()) {
            int u = stack.back();
            stack.pop_back();
            for (size_t i = 0; i < g.adj[u].size(); ++i) {
                int v = g.adj[u][i];
                if (!visited[v]) {
                    visited[v] = 1;
                    parent[v] = u;
                    stack.push_back(v);
                } else if (v != parent[u]) {
                    return false; // cycle found
                }
            }
        }
    }
    return true;
}

// Check if graph is complete
bool is_complete(const Graph& g) {
    long long expected = (long long)g.n * (g.n - 1) / 2;
    long long actual = 0;
    for (int u = 1; u <= g.n; ++u) {
        actual += (long long)g.adj[u].size();
    }
    return actual / 2 == expected;
}

TEST(DiamondFreeProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 5000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;

        if (trial % 5 == 0) {
            // Random tree (always diamond-free)
            n = 1 + std::rand() % 10;
            for (int v = 2; v <= n; ++v) {
                int u = 1 + std::rand() % (v - 1);
                edges.push_back(std::make_pair(u, v));
            }
        } else if (trial % 5 == 1) {
            // Complete graph (always diamond-free)
            n = 1 + std::rand() % 8;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 2) {
            // Diamond-prone: triangle with extra neighbor
            n = 4 + std::rand() % 6;
            // Create triangle 1-2-3
            edges.push_back(std::make_pair(1, 2));
            edges.push_back(std::make_pair(2, 3));
            edges.push_back(std::make_pair(1, 3));
            // Add vertex 4 adjacent to 2 of the triangle vertices
            edges.push_back(std::make_pair(1, 4));
            edges.push_back(std::make_pair(2, 4));
            // Random additional edges
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (std::rand() % 4 == 0)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 3) {
            // Sparse random graph
            n = 1 + std::rand() % 10;
            double p = (std::rand() % 40) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // Dense random graph
            n = 1 + std::rand() % 9;
            double p = (std::rand() % 60 + 40) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        DiamondFreeResult r1 = check_diamond_free(g, DiamondFreeAlgorithm::BRUTE);
        DiamondFreeResult r2 = check_diamond_free(g, DiamondFreeAlgorithm::EDGE_PAIR);

        ASSERT_EQ(r1.is_diamond_free, r2.is_diamond_free)
            << "BRUTE vs EDGE_PAIR trial=" << trial << " n=" << n << " m=" << edges.size();

        if (!r1.is_diamond_free) {
            ASSERT_TRUE(verify_obstruction(g, r1.obstruction))
                << "BRUTE trial=" << trial << " n=" << n;
            ASSERT_TRUE(verify_obstruction(g, r2.obstruction))
                << "EDGE_PAIR trial=" << trial << " n=" << n;
        }

        // Test 2: forest => diamond-free
        if (is_forest(g)) {
            ASSERT_TRUE(r1.is_diamond_free)
                << "forest=>diamond_free trial=" << trial << " n=" << n << " m=" << edges.size();
        }

        // Test 3: complete graph => diamond-free
        if (is_complete(g)) {
            ASSERT_TRUE(r1.is_diamond_free)
                << "complete=>diamond_free trial=" << trial << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
