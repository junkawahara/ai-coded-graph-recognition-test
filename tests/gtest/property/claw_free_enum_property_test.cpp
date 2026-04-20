#include "claw_free_enum.h"
#include "claw_free.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::ClawFreeResult;
using graph_recognition::check_claw_free;
using graph_recognition::ClawFreeEnumerationResult;
using graph_recognition::enumerate_claw_free_graphs_reverse_search;

TEST(ClawFreeEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int cf_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            ClawFreeResult res = check_claw_free(g);
            if (res.is_claw_free) {
                cf_by_filter++;
            }
        }

        // Method 2: enumeration
        ClawFreeEnumerationResult enum_res =
            enumerate_claw_free_graphs_reverse_search(n);
        int cf_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed claw-free
        for (int gi = 0; gi < cf_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            ClawFreeResult res = check_claw_free(g);
            ASSERT_TRUE(res.is_claw_free)
                << "n=" << n << " graph#" << gi << " is not claw-free";
        }

        // Compare counts
        ASSERT_EQ(cf_by_filter, cf_by_enum) << "n=" << n;
    }
}

}  // namespace
