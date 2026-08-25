#ifndef GRAPH_RECOGNITION_GTEST_CERTIFICATES_H
#define GRAPH_RECOGNITION_GTEST_CERTIFICATES_H

#include "graph.h"
#include <utility>
#include <vector>

namespace graph_recognition {

struct ChordalResult;
struct TwinQuotientResult;
struct BlockCutTreeResult;
struct MDTree;
struct TransitiveOrientationResult;

namespace gtest_utils {

// Checks that r orients every edge of g exactly once, orients nothing else,
// and that the result is transitive. Deliberately written as the naive triple
// loop rather than the library's bitset test, so a bug in that test cannot
// hide here too.
bool verify_transitive_orientation(const Graph& g, const TransitiveOrientationResult& r);

// Checks that t is a decomposition tree of g: the leaves are exactly the
// vertices, every node's vertex set is a module, the labels match the
// quotients, and substituting the children back into the quotients rebuilds
// g. expect_cotree additionally forbids PRIME nodes and requires the
// SERIES/PARALLEL labels to alternate down the tree.
bool verify_md_tree(const Graph& g, const MDTree& t, bool expect_cotree);

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

// side[v] must be 1 (clique) or 2 (independent set) for every vertex, with
// the clique side pairwise adjacent and the independent side pairwise
// non-adjacent.
bool verify_split_partition(const Graph& g, const std::vector<int>& side);

// Replays the creation sequence: the i-th vertex must be adjacent to none of
// the earlier ones (kind 0) or to all of them (kind 1). Written as the direct
// quadratic replay, independent of the library's linear-time check.
bool verify_threshold_creation_sequence(const Graph& g, const std::vector<int>& order,
                                        const std::vector<int>& kind);

// Checks a chain (complemented == false) or cochain (complemented == true)
// structure: color splits the vertices into two independent sets resp. two
// cliques, the two orderings list exactly those classes, and along each
// ordering the neighbourhoods -- resp. the non-neighbourhoods -- into the
// other class grow by inclusion.
bool verify_chain_orders(const Graph& g, const std::vector<int>& color,
                         const std::vector<int>& x_ordering,
                         const std::vector<int>& y_ordering, bool complemented);

bool verify_kregular_k(const Graph& g, int k);

bool verify_strongly_regular_params(const Graph& g, int k, int lambda, int mu);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
