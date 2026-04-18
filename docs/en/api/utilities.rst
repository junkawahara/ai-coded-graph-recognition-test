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
