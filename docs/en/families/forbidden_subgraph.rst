Forbidden Subgraph Family
=========================

Graphs defined by the absence of specific induced subgraphs.


claw_free.h -- Claw-Free Graph
-------------------------------

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


claw_free_enum.h -- Claw-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: claw_free_enum.h
   :project: graph_recognition


diamond_free.h -- Diamond-Free Graph
--------------------------------------

Determines whether a graph is diamond-free (K_4-e free).
No induced subgraph isomorphic to diamond (K_4 minus one edge).
Equivalently, any two triangles sharing an edge must form a K_4.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DiamondFreeAlgorithm``
     - Description
   * - ``BRUTE``
     - Enumerates all 4-vertex subsets to find a diamond. Complexity: O(n^4).
   * - ``EDGE_PAIR`` **(default)**
     - For each edge, checks whether common neighbors form a clique via edge counting.
       Complexity: O(nm).

.. doxygenfile:: diamond_free.h
   :project: graph_recognition


diamond_free_enum.h -- Diamond-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: diamond_free_enum.h
   :project: graph_recognition


triangle_free.h -- Triangle-Free Graph
----------------------------------------

Determines whether a graph is triangle-free.
No three mutually adjacent vertices.

.. doxygenfile:: triangle_free.h
   :project: graph_recognition


triangle_free_enum.h -- Triangle-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: triangle_free_enum.h
   :project: graph_recognition


bull_free.h -- Bull-Free Graph
-------------------------------

Determines whether a graph is bull-free.
No induced subgraph isomorphic to the bull
(a triangle with two pendant edges at distinct vertices).

.. doxygenfile:: bull_free.h
   :project: graph_recognition


bull_free_enum.h -- Bull-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: bull_free_enum.h
   :project: graph_recognition


p5_free.h -- P5-Free Graph
----------------------------

Determines whether a graph is P_5-free.
No induced path on 5 vertices.

.. doxygenfile:: p5_free.h
   :project: graph_recognition


p5_free_enum.h -- P5-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: p5_free_enum.h
   :project: graph_recognition


gem_free.h -- Gem-Free Graph
------------------------------

Determines whether a graph is gem-free.
No induced subgraph isomorphic to the gem
(a fan graph F_{1,3}, i.e., a path P_4 plus a vertex adjacent to all of P_4).

.. doxygenfile:: gem_free.h
   :project: graph_recognition


gem_free_enum.h -- Gem-Free Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: gem_free_enum.h
   :project: graph_recognition
