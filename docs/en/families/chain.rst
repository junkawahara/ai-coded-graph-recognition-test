Chain Graph
===========

Determines whether a graph is a chain graph.
A bipartite graph where the neighborhoods within each part are totally ordered by inclusion.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_442.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

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

.. doxygenenum:: graph_recognition::ChainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chain
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ChainEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChainEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chain_graphs
   :project: graph_recognition


References
----------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
