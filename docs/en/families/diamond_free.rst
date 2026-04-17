Diamond-Free Graph
==================

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

Recognition
-----------

.. doxygenfile:: diamond_free.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: diamond_free_enum.h
   :project: graph_recognition


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
