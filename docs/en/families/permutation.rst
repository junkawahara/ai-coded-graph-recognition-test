Permutation Graph
=================

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


Enumeration
-----------

.. doxygenfile:: permutation_enum.h
   :project: graph_recognition
