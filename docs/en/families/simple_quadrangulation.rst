Simple Quadrangulation
======================

A **simple quadrangulation** is a 2-connected planar graph where every face
(including the outer face) is a quadrilateral (4-cycle).

Recognition
-----------

.. doxygenenum:: graph_recognition::SimpleQuadrangulationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_simple_quadrangulation
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SimpleQuadrangulationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SimpleQuadEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SimpleQuadrangulationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_simple_quadrangulation_graphs
   :project: graph_recognition


References
----------

* G. Brinkmann, B. D. McKay. "Construction of planar triangulations with minimum degree 4."
  *Discrete Mathematics*, 301(2--3):147--163, 2005.
  `DOI:10.1016/j.disc.2005.06.019 <https://doi.org/10.1016/j.disc.2005.06.019>`_
