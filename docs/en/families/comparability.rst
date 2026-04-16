Comparability Graph
===================

Determines whether a graph is a comparability graph.
A graph whose edges can be transitively oriented.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - Description
   * - ``TRANSITIVE_ORIENTATION`` **(default)**
     - Backtracking transitive orientation solver using internal functions from permutation.h.

.. doxygenfile:: comparability.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: comparability_enum.h
   :project: graph_recognition
