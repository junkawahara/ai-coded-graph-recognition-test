Co-Comparability Graph
======================

Determines whether a graph is a co-comparability graph.
A graph whose complement is a comparability graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoComparabilityAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies comparability recognition.

Recognition
-----------

.. doxygenfile:: co_comparability.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: co_comparability_enum.h
   :project: graph_recognition


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_
