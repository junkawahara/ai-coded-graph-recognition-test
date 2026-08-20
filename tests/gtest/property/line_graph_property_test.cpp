#include "line_graph.h"
#include "claw_free.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::LineGraphAlgorithm;
using graph_recognition::LineGraphResult;
using graph_recognition::check_line_graph;
using graph_recognition::ClawFreeResult;
using graph_recognition::check_claw_free;

// ---- Independent brute-force oracle -------------------------------------
// Straight from the definition: G = L(H) for some simple graph H iff every
// vertex of G can be assigned a distinct unordered pair of "H-vertex" labels
// such that two G-vertices are adjacent iff their label pairs intersect.
// Backtracking over assignments with canonical labeling (a fresh label must
// be the smallest unused one). Both library implementations go through
// Krausz clique partitions, so this is a different characterization.
struct RootGraphSearch {
    int n;
    const std::vector<std::vector<bool>>& adj;
    std::vector<std::pair<int, int>> pairs;  // pairs[v] for v = 1..n

    RootGraphSearch(int n_, const std::vector<std::vector<bool>>& adj_)
        : n(n_), adj(adj_), pairs(n_ + 1) {}

    bool consistent(int v, int a, int b) const {
        for (int j = 1; j < v; ++j) {
            int c = pairs[j].first, d = pairs[j].second;
            bool intersects = (a == c || a == d || b == c || b == d);
            if (adj[v][j] != intersects) return false;
            if (a == c && b == d) return false;  // H must be simple
        }
        return true;
    }

    bool assign(int v, int used) {
        if (v > n) return true;
        // Candidate pairs: both labels old; one fresh (must be used+1);
        // or two fresh (must be used+1, used+2).
        for (int a = 1; a <= used; ++a) {
            for (int b = a + 1; b <= used + 1; ++b) {
                if (!consistent(v, a, b)) continue;
                pairs[v] = std::make_pair(a, b);
                if (assign(v + 1, b > used ? b : used)) return true;
            }
        }
        if (consistent(v, used + 1, used + 2)) {
            pairs[v] = std::make_pair(used + 1, used + 2);
            if (assign(v + 1, used + 2)) return true;
        }
        return false;
    }
};

bool bf_is_line_graph(int n, const std::vector<std::pair<int, int>>& edges) {
    std::vector<std::vector<bool>> adj(n + 1, std::vector<bool>(n + 1, false));
    for (size_t i = 0; i < edges.size(); ++i) {
        adj[edges[i].first][edges[i].second] = true;
        adj[edges[i].second][edges[i].first] = true;
    }
    RootGraphSearch s(n, adj);
    return s.assign(1, 0);
}

TEST(LineGraphProperty, RandomTrialsAgreeWithBruteForce) {
    std::srand(42);

    for (int trial = 0; trial < 5000; ++trial) {
        int n;
        std::vector<std::pair<int, int>> edges;
        // True when the construction guarantees a genuine line graph, in
        // which case the recognizers MUST answer yes.
        bool known_line_graph = false;

        if (trial % 5 == 0) {
            // path = L(one-vertex-longer path), so a guaranteed positive
            n = 1 + std::rand() % 8;
            for (int v = 1; v < n; ++v) {
                edges.push_back(std::make_pair(v, v + 1));
            }
            known_line_graph = true;
        } else if (trial % 5 == 1) {
            // construct actual line graph: L(H)
            int hn = 2 + std::rand() % 5;
            std::vector<std::pair<int, int>> hedges;
            for (int u = 1; u <= hn; ++u)
                for (int v = u + 1; v <= hn; ++v)
                    if (std::rand() % 3 == 0)
                        hedges.push_back(std::make_pair(u, v));
            n = (int)hedges.size();
            if (n == 0) { n = 1; }  // K1 = L(K2), still a line graph
            else {
                for (int i = 0; i < (int)hedges.size(); ++i) {
                    for (int j = i + 1; j < (int)hedges.size(); ++j) {
                        if (hedges[i].first == hedges[j].first ||
                            hedges[i].first == hedges[j].second ||
                            hedges[i].second == hedges[j].first ||
                            hedges[i].second == hedges[j].second) {
                            edges.push_back(std::make_pair(i + 1, j + 1));
                        }
                    }
                }
            }
            known_line_graph = true;
        } else if (trial % 5 == 2) {
            // dense random graph
            n = 1 + std::rand() % 7;
            double p = (std::rand() % 80 + 20) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else if (trial % 5 == 3) {
            // sparse random graph
            n = 1 + std::rand() % 7;
            double p = (std::rand() % 40) / 100.0;
            for (int u = 1; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if ((std::rand() % 1000) / 1000.0 < p)
                        edges.push_back(std::make_pair(u, v));
        } else {
            // star + random (likely not line graph)
            n = 1 + std::rand() % 6;
            for (int v = 2; v <= n; ++v) {
                edges.push_back(std::make_pair(1, v));
            }
            for (int u = 2; u <= n; ++u)
                for (int v = u + 1; v <= n; ++v)
                    if (std::rand() % 4 == 0)
                        edges.push_back(std::make_pair(u, v));
        }

        Graph g(n, edges);

        // Test 1: Two algorithm variants should agree
        LineGraphResult r1 = check_line_graph(g, LineGraphAlgorithm::BRUTE);
        LineGraphResult r2 = check_line_graph(g, LineGraphAlgorithm::KRAUSZ);

        ASSERT_EQ(r1.is_line_graph, r2.is_line_graph)
            << "BRUTE vs KRAUSZ trial=" << trial << " n=" << n << " m=" << edges.size();

        // Test 2: a graph constructed as L(H) must be recognized
        if (known_line_graph) {
            ASSERT_TRUE(r1.is_line_graph)
                << "constructed L(H) rejected trial=" << trial
                << " n=" << n << " m=" << edges.size();
        }

        // Test 3: independent definitional oracle
        ASSERT_EQ(r1.is_line_graph, bf_is_line_graph(n, edges))
            << "impl vs root-graph oracle trial=" << trial
            << " n=" << n << " m=" << edges.size();

        // Test 4: line_graph => claw_free
        if (r1.is_line_graph) {
            ClawFreeResult cf = check_claw_free(g);
            ASSERT_TRUE(cf.is_claw_free)
                << "line_graph=>claw_free trial=" << trial << " n=" << n << " m=" << edges.size();
        }
    }
}

}  // namespace
