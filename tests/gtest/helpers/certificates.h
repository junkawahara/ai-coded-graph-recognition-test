#ifndef GRAPH_RECOGNITION_GTEST_CERTIFICATES_H
#define GRAPH_RECOGNITION_GTEST_CERTIFICATES_H

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

struct ChordalResult;
struct TwinQuotientResult;
struct BlockCutTreeResult;

namespace gtest_utils {

// Checks that r really is the block decomposition of g: the blocks partition
// the edges, each block is biconnected and maximal, cut vertices and bridges
// agree with the blocks, and the block-cut structure is a forest whose edges
// are exactly the block/cut-vertex incidences.
bool verify_block_cut_tree(const Graph& g, const BlockCutTreeResult& r);

// Checks that q partitions the vertices of g into modules and that the
// quotient graph is exactly the induced quotient. It deliberately does not
// require the blocks to be twin classes: the iterated (true + false)
// contraction produces blocks that are modules but not twin classes of g
// (contracting the false twins of C4 gives K2, whose vertices are true twins,
// and the merged block is neither a clique nor an independent set).
bool verify_twin_quotient(const Graph& g, const TwinQuotientResult& q);

bool verify_interval_model(const Graph& g,
                           const std::vector<std::pair<int, int>>& intervals);

bool verify_bipartite_coloring(const Graph& g, const std::vector<int>& color);

bool verify_chordal_peo(const Graph& g, const ChordalResult& r);

bool verify_circle_dow(const Graph& g, const std::vector<int>& dow);

bool verify_convex_bipartite(const Graph& g, const std::vector<int>& color,
                             const std::vector<int>& ordering);

bool verify_biconvex_bipartite(const Graph& g, const std::vector<int>& color,
                               const std::vector<int>& x_ordering,
                               const std::vector<int>& y_ordering);

bool verify_kregular_k(const Graph& g, int k);

bool verify_strongly_regular_params(const Graph& g, int k, int lambda, int mu);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
