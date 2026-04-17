Cactus Graph
============

A graph is a **cactus** if every biconnected component is either a single edge
or a simple cycle. Equivalently, any two simple cycles share at most one vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - Biconnected component decomposition via DFS. Each component is verified to be a single edge or a simple cycle. Complexity: O(n + m).

Recognition
-----------

.. doxygenfile:: cactus.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cactus_enum.h
   :project: graph_recognition


References
----------

* F. Harary, G. E. Uhlenbeck. "On the number of husimi trees, I."
  *Proceedings of the National Academy of Sciences*, 39(4):315--322, 1953.
  `DOI:10.1073/pnas.39.4.315 <https://doi.org/10.1073/pnas.39.4.315>`_
