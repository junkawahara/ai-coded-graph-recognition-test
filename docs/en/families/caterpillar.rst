Caterpillar Graph
=================

Determines whether a graph is a caterpillar.
A tree where all vertices are within distance 1 of a central path (the spine).
Equivalently, removing all leaves yields a path (or the empty graph).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_784.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CaterpillarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_caterpillar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CaterpillarEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CaterpillarEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_caterpillar_graphs
   :project: graph_recognition


References
----------

* F. Harary, A. J. Schwenk. "The number of caterpillars."
  *Discrete Mathematics*, 6(4):359--365, 1973.
  `DOI:10.1016/0012-365X(73)90067-8 <https://doi.org/10.1016/0012-365X(73)90067-8>`_
