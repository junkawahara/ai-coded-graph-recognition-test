Block Graph
===========

A graph is a **block graph** if every biconnected component is a clique.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_93.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BlockAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - DFS-based biconnected component check, O(n + m)
   * - ``CHORDAL_DIAMOND_FREE``
     - Chordal + diamond-free recognition, O(n + m Delta)

.. doxygenenum:: graph_recognition::BlockAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_block
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BlockEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BlockEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_block_graphs_reverse_search
   :project: graph_recognition


References
----------

* F. Harary. "A characterization of block-graphs."
  *Canadian Mathematical Bulletin*, 6(1):1--6, 1963.
  `DOI:10.4153/CMB-1963-001-x <https://doi.org/10.4153/CMB-1963-001-x>`_
