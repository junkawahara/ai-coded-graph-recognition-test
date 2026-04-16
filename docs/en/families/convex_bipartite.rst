Convex Bipartite Graph
======================

Determines whether a graph is a convex bipartite graph.
A bipartite graph G=(X, Y, E) where Y can be linearly ordered
so that each vertex in X has neighbors forming a consecutive interval.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ConvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of Y to check the consecutive ones property (C1P).
       Complexity: O(|Y|! * (|X| + m)).
   * - ``C1P`` **(default)**
     - Determines C1P via partition refinement. Complexity: O(n + m).

.. doxygenfile:: convex_bipartite.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: convex_bipartite_enum.h
   :project: graph_recognition
