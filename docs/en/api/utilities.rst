Utilities
=========

dsu.h -- Union-Find
--------------------

.. doxygenstruct:: graph_recognition::DSU
   :project: graph_recognition
   :members:


mcs.h -- Maximum Cardinality Search
------------------------------------

.. doxygenenum:: graph_recognition::MCSAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MCSResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::mcs
   :project: graph_recognition


lexbfs.h -- Lexicographic BFS
------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LexBFSAlgorithm``
     - Description
   * - ``SIMPLE_LEXBFS``
     - Straightforward LexBFS. Complexity: O(n² + nm).
   * - ``PARTITION_LEXBFS`` **(default)**
     - Partition-refinement LexBFS. Complexity: O(n + m).

.. doxygenenum:: graph_recognition::LexBFSAlgorithm
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::lexbfs
   :project: graph_recognition


clique.h -- Maximal Clique Enumeration / Clique Tree
-----------------------------------------------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CliqueTreeAlgorithm``
     - Description
   * - ``KRUSKAL``
     - Maximum spanning tree of the clique intersection graph.
   * - ``INCREMENTAL`` **(default)**
     - PEO-based incremental construction.

.. doxygenenum:: graph_recognition::CliqueTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalCliques
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CliqueTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_cliques
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_clique_tree
   :project: graph_recognition


graph_utils.h -- Elementary Graph Transformations
--------------------------------------------------

Shared implementations of complement construction, adjacency matrices and
induced subgraphs. Each recognizer used to carry a private copy.

.. doxygenfunction:: graph_recognition::build_complement
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_adj_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::build_complement_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_subgraph
   :project: graph_recognition

components.h -- Connected and Complement Connected Components
---------------------------------------------------------------

Besides whole-graph entry points, subset-restricted variants are provided.
Decomposition algorithms repeatedly need the components of an induced
subgraph, so the subset variants take and return original vertex numbers.

The complement search never materializes the complement graph: it keeps a
list of unassigned vertices and moves all non-neighbors of the current vertex
at once, giving O(n + m) per call.

.. doxygenstruct:: graph_recognition::ComponentsResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::connected_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::co_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_components
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::induced_co_components
   :project: graph_recognition

twins.h -- Twin Classes and Twin Quotients
--------------------------------------------

Two distinct vertices are *true twins* when N[u] = N[v] (they are then
adjacent) and *false twins* when N(u) = N(v) (they are then non-adjacent).
Being a twin of either kind is an equivalence relation -- u, v true twins and
v, w false twins is contradictory -- so every class is a clique or an
independent set and the quotient is well defined.

The true-twin classes are exactly the **critical cliques** used by the leaf
power recognizers; the true/false fixpoint contraction is the reduction that
shrinks circle graph instances before the Naji system is built.

A single round already is a fixpoint for one kind; mixing the two needs
iteration (contracting the false twins of C4 gives K2, whose vertices are
true twins).

.. doxygenenum:: graph_recognition::TwinKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TwinQuotientResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::contract_twins
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::critical_clique_quotient
   :project: graph_recognition

block_cut_tree.h -- Biconnected Components and the Block-Cut Tree
------------------------------------------------------------------

The decomposition is computed by the usual edge-stack DFS (Hopcroft &
Tarjan 1973).

Cut vertices and bridges are not detected separately. A vertex is a cut
vertex exactly when it lies in more than one block, and a bridge is exactly a
block consisting of a single edge; deriving both from the blocks avoids the
root-of-the-DFS-tree special case that vertex-based articulation tests need.

An isolated vertex is reported as a K1 block, so every vertex belongs to a
block. A disconnected graph yields a forest.

.. doxygenstruct:: graph_recognition::BlockCutTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_block_cut_tree
   :project: graph_recognition

md_tree.h -- Decomposition Tree (shared by modular decomposition and cotrees)
------------------------------------------------------------------------------

A node of the tree stands for a module: leaves are single vertices, and an
internal node is labelled by how its children relate -- PARALLEL when no two
children are joined, SERIES when every two are, PRIME otherwise. A cotree is
exactly such a tree without PRIME nodes, so cograph.h and
modular_decomposition.h build the same structure and can be compared
directly.

Builders only create nodes (a parent always before its children) and record
leaf vertices; ``md_finalize()`` derives the vertex sets, orders the children
canonically and builds the quotient graphs.

.. doxygenenum:: graph_recognition::MDNodeKind
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MDNode
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MDTree
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::md_finalize
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::md_rebuild_graph
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::md_is_cotree
   :project: graph_recognition

transitive_orientation.h -- Transitive Orientation
----------------------------------------------------

A graph is a comparability graph exactly when its edges can be oriented so
that the result is transitive, that is when it is the comparability graph of
a partial order. comparability.h, co_comparability.h, permutation.h,
bipartite_permutation.h and trapezoid.h are all built on this header.

The public entry points verify the transitivity of the orientation they are
about to return. The check costs O(n^3 / 64) with row bitsets, negligible
next to the search, and turns a propagation bug into a loud failure rather
than a quietly wrong answer.

.. doxygenenum:: graph_recognition::TransitiveOrientationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TransitiveOrientationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::transitive_orientation
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::transitive_orientation_matrix
   :project: graph_recognition

elimination_orderings.h -- Strong Elimination Orderings
---------------------------------------------------------

A vertex is *simple* when the closed neighbourhoods of its neighbours are
linearly ordered by inclusion, and a graph is strongly chordal exactly when
it can be reduced to nothing by repeatedly deleting simple vertices. That
makes any simple-vertex elimination enough to **recognize** the class, but not
enough to be a strong elimination ordering, which is strictly stronger.

The construction therefore follows Farber: at each step the strict
closed-neighbourhood inclusions of the remaining graph are added to a partial
order, and the simple vertex removed is one minimal in it (smallest label
among ties). The result is checked against the definition before it is
returned.

An ordering v1, ..., vn is a strong elimination ordering iff for all i < j and
k < l, vk and vl in N[vi] and vk in N[vj] imply vl in N[vj] -- that is, iff
the closed neighbourhood matrix in this order contains no

  1 1
  1 0

pattern. Perfect elimination follows from it.

.. doxygenstruct:: graph_recognition::StrongEliminationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_strong_elimination_ordering
   :project: graph_recognition

tree_layout.h -- Indifference Tree-Layouts and Their Block Trees
------------------------------------------------------------------

An indifference tree-layout of a graph is a rooted tree on its vertices in
which every vertex's neighbourhood is an interval of the root-to-vertex path
extended downwards -- the tree generalization of the vertex order that
defines proper interval graphs. A chordal graph admits one exactly when it is
proper chordal (Paul & Protopapas, STACS 2024).

The construction goes through the **block tree** of a layout rooted at a
given vertex (Algorithm 1 of the paper), whose blocks are then checked for
the nested-convex condition (Algorithm 2).

Complexity: the algorithm of the paper is O(n^4), but the nested-convex
verification here enumerates all vertex orderings of a block, so the worst
case is factorial in the block size.

.. doxygenstruct:: graph_recognition::LayoutBlockTree
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TreeLayoutResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_layout_block_tree
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::find_indifference_tree_layout
   :project: graph_recognition

tree_decomposition.h -- Tree Decompositions of Chordal Graphs
---------------------------------------------------------------

A tree decomposition assigns each node of a tree a bag of vertices so that
every vertex lies in some bag, every edge lies inside some bag, and the bags
holding any one vertex form a connected subtree. Its width is the largest bag
size minus one, and the treewidth of a graph is the smallest width over all
of its tree decompositions.

For a chordal graph the clique tree already is an optimal tree decomposition:
its bags are the maximal cliques, the running intersection property is
exactly the subtree condition, and no decomposition can have a bag smaller
than the largest clique. So this header is a thin reading of ``clique.h``
rather than a search -- which is also why it only handles chordal graphs:
computing treewidth in general is NP-hard.

A disconnected graph gives a clique forest; the pieces are linked into a
tree, which changes nothing because bags from different components share no
vertex.

.. doxygenstruct:: graph_recognition::TreeDecompositionResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::tree_decomposition_from_clique_tree
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::tree_decomposition_chordal
   :project: graph_recognition

modular_decomposition.h -- Modular Decomposition Tree
-------------------------------------------------------

A *module* is a vertex set whose members are indistinguishable from the
outside: every vertex not in the set is adjacent to all of it or to none of
it. The modular decomposition tree records the **strong modules** -- those
that overlap no other module -- as a tree whose leaves are the vertices.

The construction follows Gallai's recursion. On a vertex set V:

- one vertex: a leaf;
- G[V] disconnected: a PARALLEL node whose children are the components;
- the complement disconnected: a SERIES node whose children are the
  co-components;
- both connected: a PRIME node whose children are the maximal proper modules,
  which by Gallai's theorem are pairwise disjoint and cover V.

The only nontrivial primitive is the smallest module containing a given pair.
It is computed by closure: start from the pair and repeatedly absorb any
*splitter*, a vertex adjacent to some but not all of the current set. Every
module containing the pair must contain each absorbed vertex, so the fixpoint
is the smallest one. Two vertices lie in the same maximal proper module
exactly when their closure is not all of V, and since those modules are
disjoint, union-find over that relation recovers them.

Complexity: O(n^4) in the worst case -- O(n^2) closures per prime node, each
O(n^2). The linear-time algorithms (Tedder et al.) are not implemented.

.. doxygenfunction:: graph_recognition::modular_decomposition
   :project: graph_recognition

minor.h -- Minor Checking
--------------------------

Internal utility for fixed forbidden-minor detection. All symbols live in
``graph_recognition::detail_minor`` and are not part of the public API.


pq_tree.h -- PQ-Tree and the Consecutive Ones Property
--------------------------------------------------------

PQ-tree data structure of Booth & Lueker (1976) and the consecutive ones
test built on it. ``consecutive_ones()`` is the entry point; ``PQTree``
itself is public for callers that need to reduce rows incrementally.

The implementation is correct but not linear time: the BUBBLE pass of the
original paper is not implemented, so each reduction walks from every
pertinent leaf up to the root.

Columns are numbered 1 .. num_columns, matching the library's 1-indexed
convention. (The internal ``detail::check_c1p_pq_tree`` numbers them from 0.)

.. doxygenstruct:: graph_recognition::ConsecutiveOnesResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::consecutive_ones
   :project: graph_recognition

.. doxygenenum:: graph_recognition::PQNodeType
   :project: graph_recognition

.. doxygenenum:: graph_recognition::PQLabel
   :project: graph_recognition


planar_embedding.h -- Planar Embedding
---------------------------------------

.. doxygenstruct:: graph_recognition::PlanarEmbeddingResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_planar_embedding
   :project: graph_recognition
