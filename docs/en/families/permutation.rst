Permutation / Comparability Family
===================================

Graphs defined through transitive orientations and permutation representations.
A permutation graph has edges between pairs whose order is reversed by a permutation;
comparability graphs admit a transitive orientation of their edges.


permutation.h -- Permutation Graph
------------------------------------

Determines whether a graph is a permutation graph.
For a permutation pi, vertices i and j are adjacent iff their order is reversed by pi.
Equivalent to both G and complement(G) being comparability graphs.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PermutationAlgorithm``
     - Description
   * - ``BACKTRACKING``
     - Searches for transitive orientations of both G and complement(G) via backtracking.
   * - ``CLASS_BASED`` **(default)**
     - Checks orientation consistency per Gamma class (edge equivalence class).
       Greedily selects unoriented edges and propagates constraints within each class.

.. doxygenfile:: permutation.h
   :project: graph_recognition


permutation_enum.h -- Permutation Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: permutation_enum.h
   :project: graph_recognition


comparability.h -- Comparability Graph
----------------------------------------

Determines whether a graph is a comparability graph.
A graph whose edges can be transitively oriented.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - Description
   * - ``TRANSITIVE_ORIENTATION`` **(default)**
     - Backtracking transitive orientation solver using internal functions from permutation.h.

.. doxygenfile:: comparability.h
   :project: graph_recognition


comparability_enum.h -- Comparability Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: comparability_enum.h
   :project: graph_recognition


co_comparability.h -- Co-Comparability Graph
----------------------------------------------

Determines whether a graph is a co-comparability graph.
A graph whose complement is a comparability graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies comparability recognition.

.. doxygenfile:: co_comparability.h
   :project: graph_recognition


co_comparability_enum.h -- Co-Comparability Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: co_comparability_enum.h
   :project: graph_recognition


bipartite_permutation.h -- Bipartite Permutation Graph
-------------------------------------------------------

Determines whether a graph is a bipartite permutation graph.
A graph that is both bipartite and a permutation graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartitePermutationAlgorithm``
     - Description
   * - ``CHAIN_BOTH_SIDES`` **(default)**
     - Applies both bipartite recognition and permutation recognition.

.. doxygenfile:: bipartite_permutation.h
   :project: graph_recognition


bipartite_permutation_enum.h -- Bipartite Permutation Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: bipartite_permutation_enum.h
   :project: graph_recognition


trapezoid.h -- Trapezoid Graph
-------------------------------

Determines whether a graph is a trapezoid graph.
Vertices correspond to trapezoids between two horizontal lines,
with edges between overlapping trapezoids.
Equivalent to being a co-comparability graph whose corresponding
partial order has interval dimension at most 2.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TrapezoidAlgorithm``
     - Description
   * - ``CHAIN_COVER`` **(default)**
     - Based on Cogis (1982) characterization.
       (1) Determines co-comparability via transitive orientation of the complement.
       (2) Constructs the bipartite graph B(P) of the corresponding partial order P
       (edge (x,y) iff NOT x <_P y), then checks bipartiteness of the incompatibility
       graph I(B) (edges from 2+2 patterns) via BFS.
       Trivial 2K_2 (fewer than 4 distinct elements) are excluded.

.. doxygenfile:: trapezoid.h
   :project: graph_recognition


trapezoid_enum.h -- Trapezoid Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: trapezoid_enum.h
   :project: graph_recognition
