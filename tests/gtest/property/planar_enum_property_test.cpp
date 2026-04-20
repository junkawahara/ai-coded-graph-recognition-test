#include "planar_enum.h"
#include "planar.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::PlanarResult;
using graph_recognition::check_planar;
using graph_recognition::PlanarEnumerationResult;
using graph_recognition::enumerate_planar_graphs_reverse_search;

TEST(PlanarEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute force all 2^(n choose 2) graphs + filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v)
                all_edges.push_back(std::make_pair(u, v));

        int planar_by_filter = 0;
        for (long long mask = 0; mask < (1LL << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1LL << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            PlanarResult res = check_planar(g);
            if (res.is_planar) {
                planar_by_filter++;
            }
        }

        // Method 2: enumeration
        PlanarEnumerationResult enum_res =
            enumerate_planar_graphs_reverse_search(n);
        int planar_by_enum = (int)enum_res.graphs.size();

        // Verify each enumerated graph is indeed planar
        for (int gi = 0; gi < planar_by_enum; ++gi) {
            const EnumeratedGraph& eg = enum_res.graphs[gi];
            Graph g(n, eg.edges);
            PlanarResult res = check_planar(g);
            ASSERT_TRUE(res.is_planar)
                << "n=" << n << " graph#" << gi << " is not planar";
        }

        // Compare counts
        ASSERT_EQ(planar_by_filter, planar_by_enum) << "n=" << n;
    }
}

}  // namespace
