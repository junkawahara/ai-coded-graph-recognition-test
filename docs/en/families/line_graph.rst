Line Graph
==========

A **line graph** L(H) has vertices representing edges of some graph H,
with two vertices adjacent whenever the corresponding edges share an
endpoint.  By Whitney's theorem (1932), a graph is a line graph if and only
if it admits a Krausz partition (an edge-clique cover where each vertex
belongs to at most two cliques).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``LineGraphAlgorithm``
     - Description
   * - ``BRUTE``
     - Backtracking search for a Krausz partition.  Enumerates all
       clique candidates containing each edge.  Suitable for small graphs.
   * - ``KRAUSZ`` **(default)**
     - Prerequisite filter (bipartiteness check of the complement of each
       vertex's neighborhood) followed by Krausz partition construction with
       pruning.  Complexity: O(m * Delta).

.. doxygenfile:: line_graph.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: line_graph_enum.h
   :project: graph_recognition
