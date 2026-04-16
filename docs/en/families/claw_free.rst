Claw-Free Graph
===============

Determines whether a graph is claw-free (K_{1,3}-free).
No induced subgraph isomorphic to K_{1,3} (claw).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClawFreeAlgorithm``
     - Description
   * - ``TRIPLE_LOOP``
     - Searches for an independent set of size 3 in each vertex's neighborhood.
       Complexity: O(n * Delta^3).
   * - ``EDGE_COUNT`` **(default)**
     - Uses edge counting to test neighborhood completeness;
       performs detailed search only when incomplete. Complexity: O(m * Delta).

.. doxygenfile:: claw_free.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: claw_free_enum.h
   :project: graph_recognition
