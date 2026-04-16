Distance-Hereditary Graph
=========================

A graph is **distance-hereditary** if, in every connected induced subgraph,
the distances between vertices are preserved from the original graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - Description
   * - ``HASHMAP_TWINS``
     - Iterative pendant/twin vertex removal using hash-map twin detection.
   * - ``SORTED_TWINS``
     - Iterative pendant/twin vertex removal using sorted neighbor-list
       comparison.  Deterministic.
   * - ``HASH_TWINS`` **(default)**
     - XOR hash incremental twin detection.  Each vertex receives a random
       64-bit weight; upon removal the neighbor hashes are updated in O(1).
       Hash matches are verified by exact neighbor-list comparison.
       Complexity: O(n + m) expected.

.. doxygenfile:: distance_hereditary.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: distance_hereditary_enum.h
   :project: graph_recognition
