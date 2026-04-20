#include "cograph_enum.h"
#include "cograph.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::CographResult;
using graph_recognition::check_cograph;
using graph_recognition::CographEnumerationResult;
using graph_recognition::enumerate_cograph_graphs_cotree;

TEST(CographEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 5;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: brute-force enumerate all labeled graphs + cograph filter
        int num_edges = n * (n - 1) / 2;
        std::vector<std::pair<int, int>> all_edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                all_edges.push_back(std::make_pair(u, v));
            }
        }

        int cograph_by_filter = 0;
        for (int mask = 0; mask < (1 << num_edges); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < num_edges; ++i) {
                if (mask & (1 << i)) {
                    edges.push_back(all_edges[i]);
                }
            }
            Graph g(n, edges);
            CographResult cr = check_cograph(g);
            if (cr.is_cograph) {
                cograph_by_filter++;
            }
        }

        // Method 2: direct cograph enumeration via cotree
        CographEnumerationResult cograph_res =
            enumerate_cograph_graphs_cotree(n);
        int cograph_by_enum = (int)cograph_res.graphs.size();

        // Verify each enumerated graph is indeed a cograph
        for (int gi = 0; gi < cograph_by_enum; ++gi) {
            const EnumeratedGraph& eg = cograph_res.graphs[gi];
            Graph g(n, eg.edges);
            CographResult cr = check_cograph(g);
            ASSERT_TRUE(cr.is_cograph)
                << "n=" << n << " graph#" << gi << " is not cograph";
        }

        // Compare counts
        ASSERT_EQ(cograph_by_filter, cograph_by_enum) << "n=" << n;
    }
}

}  // namespace
