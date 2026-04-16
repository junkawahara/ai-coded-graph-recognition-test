Cograph
=======

A graph is a **cograph** if it contains no induced P4 (path on 4 vertices).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CographAlgorithm``
     - Description
   * - ``COTREE``
     - Cotree construction via recursive connected-component /
       co-component decomposition.
   * - ``PARTITION_REFINEMENT`` **(default)**
     - Same decomposition with a fast linked-list co-component search.
       Complexity: O(n + m).

.. doxygenfile:: cograph.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cograph_enum.h
   :project: graph_recognition
