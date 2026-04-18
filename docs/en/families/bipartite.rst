Bipartite Graph
===============

Determines whether a graph is bipartite.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_69.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - Description
   * - ``BFS`` **(default)**
     - Attempts a 2-coloring via BFS. Detects odd cycles for non-bipartite graphs.
       Complexity: O(n + m).

.. doxygenenum:: graph_recognition::BipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_graphs_reverse_search
   :project: graph_recognition


References
----------

* D. König. *Theorie der endlichen und unendlichen Graphen.*
  Akademische Verlagsgesellschaft, Leipzig, 1936.
