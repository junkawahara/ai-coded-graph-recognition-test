Cactus Graph
============

A graph is a **cactus** if every biconnected component is either a single edge
or a simple cycle. Equivalently, any two simple cycles share at most one vertex.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_108.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - Biconnected component decomposition via DFS. Each component is verified to be a single edge or a simple cycle. Complexity: O(n + m).

.. doxygenenum:: graph_recognition::CactusAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cactus
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CactusEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CactusEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cactus_graphs_reverse_search
   :project: graph_recognition


References
----------

* F. Harary, G. E. Uhlenbeck. "On the number of husimi trees, I."
  *Proceedings of the National Academy of Sciences*, 39(4):315--322, 1953.
  `DOI:10.1073/pnas.39.4.315 <https://doi.org/10.1073/pnas.39.4.315>`_
