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

minor.h -- Minor Checking
--------------------------

Internal utility for fixed forbidden-minor detection. All symbols live in
``graph_recognition::detail_minor`` and are not part of the public API.


pq_tree.h -- PQ-Tree
---------------------

Internal utility implementing the PQ-tree data structure of Booth & Lueker
(1976) for testing the consecutive ones property. All symbols live in
``graph_recognition::detail`` and are not part of the public API.


planar_embedding.h -- Planar Embedding
---------------------------------------

.. doxygenstruct:: graph_recognition::PlanarEmbeddingResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::compute_planar_embedding
   :project: graph_recognition
