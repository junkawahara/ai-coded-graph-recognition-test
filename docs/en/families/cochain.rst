Co-Chain Graph
==============

Determines whether a graph is a co-chain graph.
A graph whose complement is a chain graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CochainAlgorithm``
     - Description
   * - ``COMPLEMENT``
     - Builds the complement and applies chain graph recognition. Complexity: O(n^2).
   * - ``DIRECT`` **(default)**
     - Complement BFS (linked-list technique) for co-bipartite detection,
       then verifies the suffix property. Complexity: O(n + m).

.. doxygenfile:: cochain.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cochain_enum.h
   :project: graph_recognition
