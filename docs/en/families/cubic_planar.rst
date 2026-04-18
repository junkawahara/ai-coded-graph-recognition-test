Cubic Planar Graph
==================

A **cubic planar graph** is a planar graph where every vertex has degree
exactly 3.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1102.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CubicPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CubicPlanarEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicPlanarEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_planar_graphs
   :project: graph_recognition


References
----------

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
