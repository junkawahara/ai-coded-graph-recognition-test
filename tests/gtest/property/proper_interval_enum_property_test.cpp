#include "chordal_enum.h"
#include "proper_interval_enum.h"
#include "proper_interval.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::ProperIntervalResult;
using graph_recognition::check_proper_interval;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::ProperIntervalEnumerationResult;
using graph_recognition::enumerate_proper_interval_graphs_reverse_search;

TEST(ProperIntervalEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + proper interval filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int pi_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            ProperIntervalResult pr = check_proper_interval(g);
            if (pr.is_proper_interval) {
                pi_by_filter++;
            }
        }

        // Method 2: direct proper interval enumeration
        ProperIntervalEnumerationResult pi_res =
            enumerate_proper_interval_graphs_reverse_search(n);
        int pi_by_enum = (int)pi_res.graphs.size();

        // Verify each enumerated graph is indeed proper interval
        for (int gi = 0; gi < pi_by_enum; ++gi) {
            const EnumeratedGraph& eg = pi_res.graphs[gi];
            Graph g(n, eg.edges);
            ProperIntervalResult pr = check_proper_interval(g);
            ASSERT_TRUE(pr.is_proper_interval)
                << "n=" << n << " graph#" << gi << " is not proper interval";
        }

        // Compare counts
        ASSERT_EQ(pi_by_filter, pi_by_enum) << "n=" << n;
    }
}

}  // namespace
