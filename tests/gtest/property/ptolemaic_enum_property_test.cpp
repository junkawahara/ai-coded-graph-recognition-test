#include "chordal_enum.h"
#include "ptolemaic_enum.h"
#include "ptolemaic.h"
#include "graph.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::EnumeratedGraph;
using graph_recognition::PtolemaicResult;
using graph_recognition::check_ptolemaic;
using graph_recognition::ChordalEnumerationResult;
using graph_recognition::enumerate_chordal_graphs_reverse_search;
using graph_recognition::PtolemaicEnumerationResult;
using graph_recognition::enumerate_ptolemaic_graphs_reverse_search;

TEST(PtolemaicEnumProperty, EnumerationAgreesWithBruteForce) {
    int max_n = 6;

    for (int n = 1; n <= max_n; ++n) {
        // Method 1: chordal enumeration + ptolemaic filter
        ChordalEnumerationResult chordal_res =
            enumerate_chordal_graphs_reverse_search(n);

        int total_chordal = (int)chordal_res.graphs.size();
        int ptolemaic_by_filter = 0;

        for (int gi = 0; gi < total_chordal; ++gi) {
            const EnumeratedGraph& eg = chordal_res.graphs[gi];
            Graph g(n, eg.edges);
            PtolemaicResult pr = check_ptolemaic(g);
            if (pr.is_ptolemaic) {
                ptolemaic_by_filter++;
            }
        }

        // Method 2: direct ptolemaic enumeration
        PtolemaicEnumerationResult ptol_res =
            enumerate_ptolemaic_graphs_reverse_search(n);
        int ptolemaic_by_enum = (int)ptol_res.graphs.size();

        // Verify each enumerated graph is indeed ptolemaic
        for (int gi = 0; gi < ptolemaic_by_enum; ++gi) {
            const EnumeratedGraph& eg = ptol_res.graphs[gi];
            Graph g(n, eg.edges);
            PtolemaicResult pr = check_ptolemaic(g);
            ASSERT_TRUE(pr.is_ptolemaic)
                << "n=" << n << " graph#" << gi << " is not ptolemaic";
        }

        // Compare counts
        ASSERT_EQ(ptolemaic_by_filter, ptolemaic_by_enum) << "n=" << n;
    }
}

}  // namespace
