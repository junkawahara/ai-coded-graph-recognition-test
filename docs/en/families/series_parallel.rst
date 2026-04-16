Series-Parallel Graph
=====================

A graph is **series-parallel** if it contains no K\ :sub:`4` minor.
Equivalently, it is a 2-degenerate graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SeriesParallelAlgorithm``
     - Description
   * - ``MINOR_CHECK``
     - Iterative removal of vertices with degree at most 2 via full scan. Complexity: O(n\ :sup:`2`).
   * - ``QUEUE_REDUCTION`` **(default)**
     - Queue-based degree-2 vertex removal (2-degeneracy test). Complexity: O(n + m).

.. doxygenfile:: series_parallel.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: series_parallel_enum.h
   :project: graph_recognition


References
----------

* R. J. Duffin. "Topology of series-parallel networks."
  *Journal of Mathematical Analysis and Applications*, 10(2):303--318, 1965.
  `DOI:10.1016/0022-247X(65)90125-3 <https://doi.org/10.1016/0022-247X(65)90125-3>`_

* J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs."
  *SIAM Journal on Computing*, 11(2):298--313, 1982.
  `DOI:10.1137/0211023 <https://doi.org/10.1137/0211023>`_
