#include "line_graph_enum.h"
#include "line_graph.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::LineGraphResult;
using graph_recognition::check_line_graph;
using graph_recognition::LineGraphEnumerationResult;
using graph_recognition::enumerate_line_graphs_reverse_search;

TEST(LineGraphEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int lg_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            LineGraphResult res = check_line_graph(g);
            if (res.is_line_graph) {
                lg_by_filter++;
            }
        }

        // Method 2: enumeration
        LineGraphEnumerationResult enum_res =
            enumerate_line_graphs_reverse_search(n);
        int lg_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed a line graph
        for (int gi = 0; gi < lg_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            LineGraphResult res = check_line_graph(g);
            ASSERT_TRUE(res.is_line_graph)
                << "n=" << n << " graph#" << gi << " is not line_graph";
        }

        // Compare counts
        ASSERT_EQ(lg_by_filter, lg_by_enum) << "n=" << n;
    }
}

}  // namespace
