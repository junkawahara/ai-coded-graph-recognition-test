Trivially Perfect Graph
=======================

A graph is **trivially perfect** if it is both chordal and a cograph.
Equivalently, for every connected induced subgraph, the graph has a universal
vertex; or equivalently, the adjacency relation coincides with the
ancestor-descendant relation in a DFS tree.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - Description
   * - **``DFS``** **(default)**
     - DFS-based recognition

.. doxygenfile:: trivially_perfect.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: trivially_perfect_enum.h
   :project: graph_recognition
