#include "decompositions/components.h"
#include "util/graph.h"
#include "decompositions/split_decomposition.h"

#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::SplitResultPair;
using graph_recognition::connected_components;
using graph_recognition::find_split;

// Oracle: every bipartition, tested straight from the definition.
bool bf_has_split(const Graph& g) {
    int n = g.n;
    if (n < 4) return false;
    for (int mask = 1; mask < (1 << n) - 1; ++mask) {
        std::vector<int> side(n + 1, 0);
        int count0 = 0;
        for (int v = 1; v <= n; ++v) {
            side[v] = ((mask >> (v - 1)) & 1) ? 0 : 1;
            if (side[v] == 0) ++count0;
        }
        if (count0 < 2 || n - count0 < 2) continue;

        std::vector<int> a1, b1;
        for (int v = 1; v <= n; ++v) {
            bool sees = false;
            for (int u = 1; u <= n; ++u) {
                if (side[u] != side[v] && g.has_edge(u, v)) sees = true;
            }
            if (!sees) continue;
            if (side[v] == 0) a1.push_back(v);
            else b1.push_back(v);
        }
        bool ok = true;
        for (size_t i = 0; i < a1.size() && ok; ++i) {
            for (size_t j = 0; j < b1.size() && ok; ++j) {
                if (!g.has_edge(a1[i], b1[j])) ok = false;
            }
        }
        if (ok) return true;
    }
    return false;
}

// The split finder is complete only if the seeding covers every split. The
// n <= 6 sweep lives in the unit test; this is the n = 7 continuation, where
// 2^21 graphs make it too slow for the default run.
TEST(FindSplitProperty, ExhaustiveOnSevenVertices) {
    const int n = 7;
    std::vector<std::pair<int, int>> all;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
    int m = (int)all.size();
    for (int mask = 0; mask < (1 << m); ++mask) {
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < m; ++i)
            if (mask & (1 << i)) edges.push_back(all[i]);
        Graph g(n, edges);
        if (connected_components(g).count != 1) continue;

        SplitResultPair r = find_split(g);
        ASSERT_EQ(r.found, bf_has_split(g)) << "mask=" << mask;
        if (r.found) {
            ASSERT_TRUE(graph_recognition::detail_split::is_split(g, r.side)) << "mask=" << mask;
        }
    }
}

}  // namespace
