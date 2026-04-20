#include "chordal_enum.h"
#include "interval_enum.h"
#include "interval.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::IntervalResult;
using graph_recognition::check_interval;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::IntervalEnumerationResult;
using graph_recognition::enumerate_interval_graphs_reverse_search;

TEST(IntervalEnumFullProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + interval filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int interval_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            IntervalResult ir = check_interval(g);
            if (ir.is_interval) {
                interval_by_filter++;
            }
        }

        // Method 2: direct interval enumeration
        IntervalEnumerationResult interval_res =
            enumerate_interval_graphs_reverse_search(n);
        int interval_by_enum = (int)interval_res.graphs.size();

        // Verify each enumerated graph is indeed interval
        for (int gi = 0; gi < interval_by_enum; ++gi) {
            const EnumeratedGraph& eg = interval_res.graphs[gi];
            Graph g(n, eg.edges);
            IntervalResult ir = check_interval(g);
            ASSERT_TRUE(ir.is_interval)
                << "n=" << n << " graph#" << gi << " is not interval";
        }

        // Compare counts
        ASSERT_EQ(interval_by_filter, interval_by_enum) << "n=" << n;
    }
}

}  // namespace
