Utilities
=========

dsu.h -- Union-Find
--------------------

.. doxygenfile:: dsu.h
   :project: graph_recognition


mcs.h -- Maximum Cardinality Search
------------------------------------

.. doxygenfile:: mcs.h
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

.. doxygenfile:: lexbfs.h
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

.. doxygenfile:: clique.h
   :project: graph_recognition


minor.h -- Minor Checking
--------------------------

.. doxygenfile:: minor.h
   :project: graph_recognition


pq_tree.h -- PQ-Tree
---------------------

.. doxygenfile:: pq_tree.h
   :project: graph_recognition


planar_embedding.h -- Planar Embedding
---------------------------------------

.. doxygenfile:: planar_embedding.h
   :project: graph_recognition
