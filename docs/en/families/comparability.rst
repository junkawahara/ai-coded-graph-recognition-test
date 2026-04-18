Comparability Graph
===================

Determines whether a graph is a comparability graph.
A graph whose edges can be transitively oriented.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_72.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ComparabilityAlgorithm``
     - Description
   * - ``TRANSITIVE_ORIENTATION`` **(default)**
     - Backtracking transitive orientation solver using internal functions from permutation.h.

.. doxygenenum:: graph_recognition::ComparabilityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_comparability
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ComparabilityEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ComparabilityEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_comparability_graphs_reverse_search
   :project: graph_recognition


References
----------

* T. Gallai. "Transitiv orientierbare Graphen."
  *Acta Mathematica Academiae Scientiarum Hungaricae*, 18(1--2):25--66, 1967.
  `DOI:10.1007/BF02020961 <https://doi.org/10.1007/BF02020961>`_

* M. C. Golumbic. *Algorithmic Graph Theory and Perfect Graphs.*
  Academic Press, 1980; 2nd edition, Annals of Discrete Mathematics 57, Elsevier, 2004.
  `DOI:10.1016/S0167-5060(04)80053-0 <https://doi.org/10.1016/S0167-5060(04)80053-0>`_
