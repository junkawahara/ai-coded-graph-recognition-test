Bipartite Family
================

Graphs whose vertex set can be partitioned into two independent sets,
along with subclasses defined by structural constraints on the bipartition.


bipartite.h -- Bipartite Graph
-------------------------------

Determines whether a graph is bipartite.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartiteAlgorithm``
     - Description
   * - ``BFS`` **(default)**
     - Attempts a 2-coloring via BFS. Detects odd cycles for non-bipartite graphs.
       Complexity: O(n + m).

.. doxygenfile:: bipartite.h
   :project: graph_recognition


bipartite_enum.h -- Bipartite Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: bipartite_enum.h
   :project: graph_recognition


chordal_bipartite.h -- Chordal Bipartite Graph
------------------------------------------------

Determines whether a graph is a chordal bipartite graph.
A bipartite graph with no induced cycle of length 6 or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Verifies bipartiteness, then brute-force searches for induced even cycles of length >= 6.
   * - ``BISIMPLICIAL``
     - Detects and removes bisimplicial edges one at a time via exhaustive search.
       Complexity: O(m * n^2).
   * - ``FAST_BISIMPLICIAL`` **(default)**
     - Detects and removes bisimplicial edges using adjacency lists + adjacency matrix.
       Complexity: O(m * deg^2).

.. doxygenfile:: chordal_bipartite.h
   :project: graph_recognition


chordal_bipartite_enum.h -- Chordal Bipartite Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chordal_bipartite_enum.h
   :project: graph_recognition


convex_bipartite.h -- Convex Bipartite Graph
----------------------------------------------

Determines whether a graph is a convex bipartite graph.
A bipartite graph G=(X, Y, E) where Y can be linearly ordered
so that each vertex in X has neighbors forming a consecutive interval.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ConvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of Y to check the consecutive ones property (C1P).
       Complexity: O(|Y|! * (|X| + m)).
   * - ``C1P`` **(default)**
     - Determines C1P via partition refinement. Complexity: O(n + m).

.. doxygenfile:: convex_bipartite.h
   :project: graph_recognition


convex_bipartite_enum.h -- Convex Bipartite Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: convex_bipartite_enum.h
   :project: graph_recognition


biconvex_bipartite.h -- Biconvex Bipartite Graph
--------------------------------------------------

Determines whether a graph is a biconvex bipartite graph.
A bipartite graph G=(X, Y, E) where both sides can be linearly ordered
so that each vertex's neighbors on the opposite side form a consecutive interval.
Both rows and columns of the biadjacency matrix satisfy the consecutive ones property.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BiconvexBipartiteAlgorithm``
     - Description
   * - ``BRUTE_FORCE``
     - Tries all permutations of both sides. Complexity: O(|X|! * |Y|! * (n + m)).
   * - ``C1P`` **(default)**
     - PQ-tree (Booth & Lueker 1976) to check C1P on both sides. Complexity: O(n + m).

.. doxygenfile:: biconvex_bipartite.h
   :project: graph_recognition


biconvex_bipartite_enum.h -- Biconvex Bipartite Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: biconvex_bipartite_enum.h
   :project: graph_recognition


chain.h -- Chain Graph
-----------------------

Determines whether a graph is a chain graph.
A bipartite graph where the neighborhoods within each part are totally ordered by inclusion.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChainAlgorithm``
     - Description
   * - ``NEIGHBORHOOD_INCLUSION``
     - Checks all pairs for neighborhood inclusion. Complexity: O(n * m).
   * - ``DEGREE_SORT`` **(default)**
     - Sorts L-side vertices by degree (counting sort) and verifies that each R-side
       vertex's L-side neighbors form a suffix. Complexity: O(n + m).

.. doxygenfile:: chain.h
   :project: graph_recognition


chain_enum.h -- Chain Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: chain_enum.h
   :project: graph_recognition


cochain.h -- Co-Chain Graph
-----------------------------

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


cochain_enum.h -- Co-Chain Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: cochain_enum.h
   :project: graph_recognition
