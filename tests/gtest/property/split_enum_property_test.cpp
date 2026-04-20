#include "chordal_enum.h"
#include "split_enum.h"
#include "split.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::SplitResult;
using graph_recognition::check_split;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::SplitEnumerationResult;
using graph_recognition::enumerate_split_graphs_reverse_search;

TEST(SplitEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + split filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int split_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            SplitResult sr = check_split(g);
            if (sr.is_split) {
                split_by_filter++;
            }
        }

        // Method 2: direct split enumeration
        SplitEnumerationResult split_res =
            enumerate_split_graphs_reverse_search(n);
        int split_by_enum = (int)split_res.graphs.size();

        // Verify each enumerated graph is indeed split
        for (int gi = 0; gi < split_by_enum; ++gi) {
            const EnumeratedGraph& eg = split_res.graphs[gi];
            Graph g(n, eg.edges);
            SplitResult sr = check_split(g);
            ASSERT_TRUE(sr.is_split)
                << "n=" << n << " graph#" << gi << " is not split";
        }

        // Compare counts
        ASSERT_EQ(split_by_filter, split_by_enum) << "n=" << n;
    }
}

}  // namespace
