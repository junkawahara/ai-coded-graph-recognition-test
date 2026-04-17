Bipartite Graph
===============

Determines whether a graph is bipartite.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - Description
   * - ``BFS`` **(default)**
     - Attempts a 2-coloring via BFS. Detects odd cycles for non-bipartite graphs.
       Complexity: O(n + m).

Recognition
-----------

.. doxygenfile:: bipartite.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: bipartite_enum.h
   :project: graph_recognition


References
----------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.
