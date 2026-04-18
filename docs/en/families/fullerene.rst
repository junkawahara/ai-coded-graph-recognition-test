Fullerene Graph
===============

A **fullerene graph** is a 3-connected cubic planar graph where every face is
either a pentagon or a hexagon. By Euler's formula, every fullerene has exactly
12 pentagonal faces.

Recognition
-----------

.. doxygenenum:: graph_recognition::FullereneAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_fullerene
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::FullereneEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::FullereneEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::FullereneEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_fullerene_graphs
   :project: graph_recognition


References
----------

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "The generation of fullerenes."
  *Journal of Chemical Information and Modeling*, 52(11):2910--2918, 2012.
  `DOI:10.1021/ci3003107 <https://doi.org/10.1021/ci3003107>`_
