Co-Comparability Graph
======================

Determines whether a graph is a co-comparability graph.
A graph whose complement is a comparability graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_147.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies comparability recognition.

.. doxygenenum:: graph_recognition::CoComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_comparability
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CoComparabilityEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoComparabilityEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_comparability_graphs_reverse_search
   :project: graph_recognition


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_
