Chordal Bipartite Graph
=======================

A graph is **chordal bipartite** if it is bipartite and contains no induced
cycle of length six or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Brute-force induced even cycle search
   * - ``BISIMPLICIAL``
     - Bisimplicial edge elimination, O(m n^2)
   * - **``FAST_BISIMPLICIAL``** **(default)**
     - Fast bisimplicial edge elimination, O(m deg^2)

.. doxygenfile:: chordal_bipartite.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: chordal_bipartite_enum.h
   :project: graph_recognition
