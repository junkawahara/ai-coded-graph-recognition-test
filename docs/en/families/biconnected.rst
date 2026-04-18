Biconnected Graph
=================

Determines whether a graph is biconnected (2-connected).
A connected graph with no cut vertex.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_771.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::BiconnectedAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_biconnected
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BiconnectedEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BiconnectedEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_biconnected_graphs
   :project: graph_recognition


References
----------

* R. Tarjan. "Depth-first search and linear graph algorithms."
  *SIAM Journal on Computing*, 1(2):146--160, 1972.
  `DOI:10.1137/0201010 <https://doi.org/10.1137/0201010>`_
