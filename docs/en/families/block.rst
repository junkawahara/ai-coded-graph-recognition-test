Block Graph
===========

A graph is a **block graph** if every biconnected component is a clique.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - Description
   * - **``DFS``** **(default)**
     - DFS-based biconnected component check, O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - Chordal + diamond-free recognition, O(n + m Delta)

Recognition
-----------

.. doxygenfile:: block.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: block_enum.h
   :project: graph_recognition


References
----------

* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
