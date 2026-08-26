#include "util/graph.h"
#include "recognizers/ktree.h"

#include "certificates.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::KTreeResult;
using graph_recognition::check_ktree;
using graph_recognition::gtest_utils::verify_ktree_construction;

// Every graph on up to 6 vertices: whenever the recognizer says k-tree, the
// construction order it reports must replay against the graph. The order is
// the simplicial elimination reversed, so this also checks the elimination.
TEST(KTreeProperty, ConstructionOrderReplaysOnAllSmallGraphs) {
    for (int n = 1; n <= 6; ++n) {
        std::vector<std::pair<int, int>> all;
        for (int u = 1; u <= n; ++u)
            for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
        int m = (int)all.size();
        for (int mask = 0; mask < (1 << m); ++mask) {
            std::vector<std::pair<int, int>> edges;
            for (int i = 0; i < m; ++i)
                if (mask & (1 << i)) edges.push_back(all[i]);
            Graph g(n, edges);

            KTreeResult r = check_ktree(g);
            if (!r.is_ktree) continue;
            ASSERT_TRUE(verify_ktree_construction(g, r.k, r.construction_order))
                << "n=" << n << " mask=" << mask << " k=" << r.k;
        }
    }
}

}  // namespace
