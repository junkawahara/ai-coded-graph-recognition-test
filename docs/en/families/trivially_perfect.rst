Trivially Perfect Graph
=======================

A graph is **trivially perfect** if it is both chordal and a cograph.
Equivalently, for every connected induced subgraph, the graph has a universal
vertex; or equivalently, the adjacency relation coincides with the
ancestor-descendant relation in a DFS tree.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_327.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TriviallyPerfectAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based recognition

.. doxygenenum:: graph_recognition::TriviallyPerfectAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_trivially_perfect
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TriviallyPerfectEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TriviallyPerfectEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trivially_perfect_graphs_uvd
   :project: graph_recognition


References
----------

* E. S. Wolk. "The comparability graph of a tree."
  *Proceedings of the American Mathematical Society*, 13(5):789--795, 1962.
  `DOI:10.1090/S0002-9939-1962-0172273-0 <https://doi.org/10.1090/S0002-9939-1962-0172273-0>`_

* M. C. Golumbic. "Trivially perfect graphs."
  *Discrete Mathematics*, 24(1):105--107, 1978.
  `DOI:10.1016/0012-365X(78)90178-4 <https://doi.org/10.1016/0012-365X(78)90178-4>`_
