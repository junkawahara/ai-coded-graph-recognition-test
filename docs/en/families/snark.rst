Snark
=====

Determines whether a graph is a snark.
A bridgeless cubic graph that is not 3-edge-colorable.

Recognition
-----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SnarkEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_graphs
   :project: graph_recognition


References
----------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_
