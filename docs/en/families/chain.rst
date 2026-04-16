Chain Graph
===========

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


Enumeration
-----------

.. doxygenfile:: chain_enum.h
   :project: graph_recognition


References
----------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
