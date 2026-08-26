#include "recognizers/chordal_bipartite.h"
#include "enumerators/chordal_bipartite_induced_subgraph_enum.h"
#include "util/graph.h"
#include <gtest/gtest.h>

#include <set>
#include <utility>
#include <vector>

namespace {

using graph_recognition::ChordalBipartiteInducedSubgraphEnumerationResult;
using graph_recognition::Graph;
using graph_recognition::check_chordal_bipartite;
using graph_recognition::enumerate_chordal_bipartite_induced_subgraphs;

typedef std::vector<std::pair<int, int> > EdgeList;
typedef std::vector<int> VertexSet;

Graph induced(const Graph& g, const VertexSet& vs) {
    std::vector<int> label(g.n + 1, 0);
    for (std::size_t i = 0; i < vs.size(); ++i) {
        label[vs[i]] = static_cast<int>(i) + 1;
    }
    EdgeList edges;
    for (std::size_t i = 0; i < vs.size(); ++i) {
        for (std::size_t j = i + 1; j < vs.size(); ++j) {
            if (g.has_edge(vs[i], vs[j])) {
                edges.push_back(std::make_pair(label[vs[i]], label[vs[j]]));
            }
        }
    }
    return Graph(static_cast<int>(vs.size()), edges);
}

std::set<VertexSet> brute_force_solutions(const Graph& g) {
    std::set<VertexSet> out;
    for (unsigned long mask = 0; mask < (1UL << g.n); ++mask) {
        VertexSet vs;
        for (int i = 0; i < g.n; ++i) {
            if (mask & (1UL << i)) vs.push_back(i + 1);
        }
        if (check_chordal_bipartite(induced(g, vs)).is_chordal_bipartite) {
            out.insert(vs);
        }
    }
    return out;
}

// Method 1 brute-forces ALL 2^n vertex subsets of the host through the
// recognizer (no enumerator in the loop) and Method 2 is the reverse search;
// the two are compared as sets of vertex sets, so duplicates, omissions, and
// invalid subgraphs are all caught individually. The edge density varies per
// trial because dense hosts and sparse hosts exercise different parts of the
// weak-simplicial test.
TEST(ChordalBipartiteInducedSubgraphEnumProperty, EnumerationAgreesWithBruteForce) {
    unsigned int seed = 20260827u;
    for (int trial = 0; trial < 80; ++trial) {
        seed = seed * 1103515245u + 12345u;
        const int n = 1 + static_cast<int>((seed >> 16) % 9u);
        seed = seed * 1103515245u + 12345u;
        const unsigned int density = 1u + (seed >> 16) % 3u;  // 1/4, 2/4 or 3/4

        EdgeList edges;
        for (int u = 1; u <= n; ++u) {
            for (int v = u + 1; v <= n; ++v) {
                seed = seed * 1103515245u + 12345u;
                if ((seed >> 16) % 4u < density) {
                    edges.push_back(std::make_pair(u, v));
                }
            }
        }
        const Graph g(n, edges);

        const ChordalBipartiteInducedSubgraphEnumerationResult res =
            enumerate_chordal_bipartite_induced_subgraphs(g);

        std::set<VertexSet> enumerated;
        for (std::size_t i = 0; i < res.vertex_sets.size(); ++i) {
            EXPECT_TRUE(enumerated.insert(res.vertex_sets[i]).second)
                << "duplicate vertex set in trial=" << trial;
        }
        EXPECT_EQ(enumerated, brute_force_solutions(g))
            << "trial=" << trial << " n=" << n << " m=" << edges.size();
    }
}

}  // namespace
