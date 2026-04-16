Cactus Graph
============

A graph is a **cactus** if every biconnected component is either a single edge
or a simple cycle. Equivalently, any two simple cycles share at most one vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - Biconnected component decomposition via DFS. Each component is verified to be a single edge or a simple cycle. Complexity: O(n + m).

.. doxygenfile:: cactus.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cactus_enum.h
   :project: graph_recognition
