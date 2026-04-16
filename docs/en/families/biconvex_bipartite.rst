Biconvex Bipartite Graph
========================

Determines whether a graph is a biconvex bipartite graph.
A bipartite graph G=(X, Y, E) where both sides can be linearly ordered
so that each vertex's neighbors on the opposite side form a consecutive interval.
Both rows and columns of the biadjacency matrix satisfy the consecutive ones property.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BiconvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of both sides. Complexity: O(|X|! * |Y|! * (n + m)).
   * - ``C1P`` **(default)**
     - PQ-tree (Booth & Lueker 1976) to check C1P on both sides. Complexity: O(n + m).

.. doxygenfile:: biconvex_bipartite.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: biconvex_bipartite_enum.h
   :project: graph_recognition
