Comparability Graph
===================

Determines whether a graph is a comparability graph.
A graph whose edges can be transitively oriented.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - Description
   * - ``TRANSITIVE_ORIENTATION`` **(default)**
     - Backtracking transitive orientation solver using internal functions from permutation.h.

Recognition
-----------

.. doxygenfile:: comparability.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: comparability_enum.h
   :project: graph_recognition


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.*
  Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004.
  `DOI:10.1016/S0167-5060(04)80013-6 <https://doi.org/10.1016/S0167-5060(04)80013-6>`_
