Laman Graph
===========

Determines whether a graph is a Laman graph.
A graph satisfying the Laman condition: m = 2n - 3 and every subgraph on k vertices
has at most 2k - 3 edges. Laman graphs characterize minimally rigid frameworks in the plane.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1206.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LamanEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_graphs
   :project: graph_recognition


References
----------

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation and pseudotriangulation."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_
