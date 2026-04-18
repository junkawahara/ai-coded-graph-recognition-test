Split Graph
===========

A graph is a **split graph** if its vertex set can be partitioned into a clique
and an independent set.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_39.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n^2)
   * - ``HAMMER_SIMEONE`` **(default)**
     - Hammer-Simeone degree-sequence condition, O(n + m)

.. doxygenenum:: graph_recognition::SplitAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_split
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SplitEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_graphs_reverse_search
   :project: graph_recognition


References
----------

* P. L. Hammer, B. Simeone. "The splittance of a graph."
  *Combinatorica*, 1(3):275--284, 1981.
  `DOI:10.1007/BF02579333 <https://doi.org/10.1007/BF02579333>`_

* S. Földes, P. L. Hammer. "Split graphs."
  *Congressus Numerantium*, 19:311--315, 1977.
