Strongly Regular Graph
======================

Determines whether a graph is strongly regular with parameters (n, k, lambda, mu).
A k-regular graph where every pair of adjacent vertices has exactly lambda common neighbors
and every pair of non-adjacent vertices has exactly mu common neighbors.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1185.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::StronglyRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_regular
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::StronglyRegularEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_graphs
   :project: graph_recognition


References
----------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_
