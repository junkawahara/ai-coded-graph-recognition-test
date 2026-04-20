#include "chordal_enum.h"
#include "block_enum.h"
#include "block.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::BlockResult;
using graph_recognition::check_block;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::BlockEnumerationResult;
using graph_recognition::enumerate_block_graphs_reverse_search;

TEST(BlockEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + block filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int block_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            BlockResult br = check_block(g);
            if (br.is_block) {
                block_by_filter++;
            }
        }

        // Method 2: direct block enumeration
        BlockEnumerationResult block_res =
            enumerate_block_graphs_reverse_search(n);
        int block_by_enum = (int)block_res.graphs.size();

        // Verify each enumerated graph is indeed block
        for (int gi = 0; gi < block_by_enum; ++gi) {
            const EnumeratedGraph& eg = block_res.graphs[gi];
            Graph g(n, eg.edges);
            BlockResult br = check_block(g);
            ASSERT_TRUE(br.is_block)
                << "n=" << n << " graph#" << gi << " is not block";
        }

        // Compare counts
        ASSERT_EQ(block_by_filter, block_by_enum) << "n=" << n;
    }
}

}  // namespace
