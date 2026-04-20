#include "co_comparability_enum.h"
#include "co_comparability.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::CoComparabilityResult;
using graph_recognition::check_co_comparability;
using graph_recognition::CoComparabilityEnumerationResult;
using graph_recognition::enumerate_co_comparability_graphs_reverse_search;

TEST(CoComparabilityEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int co_comp_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            CoComparabilityResult res = check_co_comparability(g);
            if (res.is_co_comparability) {
                co_comp_by_filter++;
            }
        }

        // Method 2: enumeration
        CoComparabilityEnumerationResult enum_res =
            enumerate_co_comparability_graphs_reverse_search(n);
        int co_comp_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed co-comparability
        for (int gi = 0; gi < co_comp_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            CoComparabilityResult res = check_co_comparability(g);
            ASSERT_TRUE(res.is_co_comparability)
                << "n=" << n << " graph#" << gi << " is not co-comparability";
        }

        // Compare counts
        ASSERT_EQ(co_comp_by_filter, co_comp_by_enum) << "n=" << n;
    }
}

}  // namespace
