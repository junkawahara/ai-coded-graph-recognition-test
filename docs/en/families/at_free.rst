AT-Free Graph
=============

A graph is **AT-free** (asteroidal triple-free) if it contains no asteroidal
triple -- three vertices such that each pair is connected by a path that
avoids the closed neighborhood of the third vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - Description
   * - ``BRUTE_FORCE`` **(default)**
     - Checks all triples (u, v, w) via BFS to determine whether each pair
       can be connected by a path avoiding the neighborhood of the third.

.. doxygenfile:: at_free.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: at_free_enum.h
   :project: graph_recognition
