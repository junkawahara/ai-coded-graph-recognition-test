Co-Chain Graph
==============

Determines whether a graph is a co-chain graph.
A graph whose complement is a chain graph.

Recognition
-----------

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

.. doxygenenum:: graph_recognition::CochainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CochainResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cochain
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::CochainEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CochainEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cochain_graphs
   :project: graph_recognition


References
----------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
