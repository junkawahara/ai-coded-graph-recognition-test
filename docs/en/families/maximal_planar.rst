Maximal Planar Graph
====================

A **maximal planar graph** (triangulation) is a planar graph where adding any
edge would violate planarity. In a maximal planar graph, every face
(including the outer face) is a triangle.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_981.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_graphs_reverse_search
   :project: graph_recognition


References
----------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_
