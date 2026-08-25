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
struct SPReduction;
struct LineGraphResult;
struct TreeDecompositionResult;

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

// Replays a k-tree construction: the first k+1 vertices must form a clique,
// and every later vertex must attach to exactly k earlier ones that form a
// clique.
bool verify_ktree_construction(const Graph& g, int k, const std::vector<int>& order);

// Replays a series-parallel reduction on a mutable copy of g: every step must
// be legal for the vertex's current degree, and all n vertices must go.
bool verify_sp_reduction(const Graph& g, const std::vector<SPReduction>& steps);

// Checks the Krausz partition (each part a clique of g, every edge in exactly
// one part, every vertex in at most two) and that the reported root graph's
// line graph really is g, read off vertex_to_root_edge.
bool verify_krausz_partition(const Graph& g, const LineGraphResult& r);

// Checks an arc model on a circle of `len` slots: arcs[v] covers slots
// arcs[v].first .. arcs[v].second - 1 clockwise modulo len, two vertices are
// adjacent exactly when their slot sets meet, and for a proper model no arc
// contains another.
bool verify_circular_arc_model(const Graph& g,
                               const std::vector<std::pair<int, int>>& arcs, int len,
                               bool proper);

// Checks the defining property of a strong elimination ordering directly:
// for positions i < j and k < l, if vk and vl lie in N[vi] and vk lies in
// N[vj], then vl must lie in N[vj].
bool verify_seo(const Graph& g, const std::vector<int>& order,
                const std::vector<int>& number);

// Checks an indifference tree-layout: parent must describe a forest on the
// vertices, every edge of g must join an ancestor to a descendant, and on any
// ancestor chain x < y < z an edge xz must force xy and yz (Paul &
// Protopapas, Theorem 6).
bool verify_tree_layout(const Graph& g, const std::vector<int>& parent);

// Checks an indifference ordering: order must be a permutation matching
// number, and every closed neighbourhood must occupy consecutive positions.
bool verify_indifference_order(const Graph& g, const std::vector<int>& order,
                               const std::vector<int>& number);

// Checks a tree decomposition: the structure is a tree, every vertex is in a
// bag, every edge is inside a bag, the bags holding any one vertex form a
// connected subtree, and width is the largest bag size minus one.
bool verify_tree_decomposition(const Graph& g, const TreeDecompositionResult& r);

bool verify_kregular_k(const Graph& g, int k);

bool verify_strongly_regular_params(const Graph& g, int k, int lambda, int mu);

}  // namespace gtest_utils
}  // namespace graph_recognition

#endif
