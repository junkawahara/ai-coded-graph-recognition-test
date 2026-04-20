#include "trivially_perfect_enum.h"
#include "trivially_perfect.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::TriviallyPerfectResult;
using graph_recognition::check_trivially_perfect;
using graph_recognition::TriviallyPerfectEnumerationResult;
using graph_recognition::enumerate_trivially_perfect_graphs_uvd;

TEST(TriviallyPerfectEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 5;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute-force enumerate all labeled graphs + TP filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                all_edges.push_back(std::make_pair(u, v));
            }
        }

        int tp_by_filter = 0;
        for (int mask = 0; mask < (1 << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1 << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            TriviallyPerfectResult tr = check_trivially_perfect(g);
            if (tr.is_trivially_perfect) {
                tp_by_filter++;
            }
        }

        // Method 2: direct TP enumeration via UVD tree
        TriviallyPerfectEnumerationResult tp_res =
            enumerate_trivially_perfect_graphs_uvd(n);
        int tp_by_enum = (int)tp_res.graphs.size();

        // Verify each enumerated graph is indeed trivially perfect
        for (int gi = 0; gi < tp_by_enum; ++gi) {
            const EnumeratedGraph& eg = tp_res.graphs[gi];
            Graph g(n, eg.edges);
            TriviallyPerfectResult tr = check_trivially_perfect(g);
            ASSERT_TRUE(tr.is_trivially_perfect)
                << "n=" << n << " graph#" << gi << " is not trivially perfect";
        }

        // Compare counts
        ASSERT_EQ(tp_by_filter, tp_by_enum) << "n=" << n;
    }
}

}  // namespace
