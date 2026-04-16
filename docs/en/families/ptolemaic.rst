Ptolemaic Graph
===============

A graph is **ptolemaic** if it is both chordal and distance-hereditary.
Equivalently, the graph is gem-free and chordal.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - Description
   * - ``DH_HASHMAP``
     - Distance-hereditary check using hash maps
   * - **``DH_SORTED``** **(default)**
     - Distance-hereditary check using sorted adjacency lists

.. doxygenfile:: ptolemaic.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: ptolemaic_enum.h
   :project: graph_recognition
