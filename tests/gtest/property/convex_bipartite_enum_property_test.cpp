#include "convex_bipartite_enum.h"
#include "convex_bipartite.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::ConvexBipartiteResult;
using graph_recognition::check_convex_bipartite;
using graph_recognition::ConvexBipartiteEnumerationResult;
using graph_recognition::enumerate_convex_bipartite_graphs_reverse_search;

TEST(ConvexBipartiteEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int cb_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            ConvexBipartiteResult res = check_convex_bipartite(g);
            if (res.is_convex_bipartite) {
                cb_by_filter++;
            }
        }

        // Method 2: enumeration
        ConvexBipartiteEnumerationResult enum_res =
            enumerate_convex_bipartite_graphs_reverse_search(n);
        int cb_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed convex bipartite
        for (int gi = 0; gi < cb_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            ConvexBipartiteResult res = check_convex_bipartite(g);
            ASSERT_TRUE(res.is_convex_bipartite)
                << "n=" << n << " graph#" << gi << " is not convex bipartite";
        }

        // Compare counts
        ASSERT_EQ(cb_by_filter, cb_by_enum) << "n=" << n;
    }
}

}  // namespace
