Weakly Chordal Graph
====================

A graph is **weakly chordal** if neither the graph nor its complement contains
an induced cycle of length five or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - Description
   * - ``CO_CHORDAL_BIPARTITE``
     - Co-chordal-bipartite based recognition, O(n^2 + n m)
   * - **``COMPLEMENT_BFS``** **(default)**
     - Complement BFS approach, O(n m)

.. doxygenfile:: weakly_chordal.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: weakly_chordal_enum.h
   :project: graph_recognition
