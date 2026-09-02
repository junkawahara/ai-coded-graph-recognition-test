#include "recognizers/proper_chordal.h"
#include "certificates.h"
#include "decompositions/tree_layout.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <utility>
#include <vector>

namespace {

using graph_recognition::Graph;
using graph_recognition::check_proper_chordal;
using graph_recognition::gtest_utils::verify_tree_layout;

// Brute force over every rooted forest on {1, ..., n}, checked against the
// definition of an indifference tree-layout by the test helpers' own verifier
// (a forest layout implies a rooted one: no graph edge can cross two trees of
// the forest, so their roots can be hung together freely).
bool bf_is_proper_chordal(const Graph& g) {
    const int n = g.n;
    std::vector<int> parent(n + 1, 0);
    long long total = 1;
    for (int i = 0; i < n; ++i) total *= (n + 1);
    for (long long code = 0; code < total; ++code) {
        long long rest = code;
        for (int v = 1; v <= n; ++v) {
            parent[v] = static_cast<int>(rest % (n + 1));
            rest /= (n + 1);
        }
        if (verify_tree_layout(g, parent)) return true;
    }
    return false;
}

// The recognizer decides membership by searching for a layout; the oracle
// decides it by exhausting every rooted forest. Both answers, every graph.
TEST(ProperChordalProperty, ExhaustiveOracleAgreementOnFiveVertices) {
    const int n = 5;
    std::vector<std::pair<int, int>> all;
    for (int u = 1; u <= n; ++u)
        for (int v = u + 1; v <= n; ++v) all.push_back(std::make_pair(u, v));
    const int m = static_cast<int>(all.size());
    for (int mask = 0; mask < (1 << m); ++mask) {
        std::vector<std::pair<int, int>> edges;
        for (int i = 0; i < m; ++i)
            if (mask & (1 << i)) edges.push_back(all[i]);
        Graph g(n, edges);
        ASSERT_EQ(check_proper_chordal(g).is_proper_chordal,
                  bf_is_proper_chordal(g))
            << "mask=" << mask;
    }
}

}  // namespace
