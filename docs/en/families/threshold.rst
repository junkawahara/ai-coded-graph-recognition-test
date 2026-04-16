Threshold Graph
===============

A graph is a **threshold graph** if it can be reduced to the empty graph by
repeatedly removing an isolated vertex or a universal vertex (a vertex adjacent
to all others).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ThresholdAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n m)
   * - **``DEGREE_SEQUENCE_FAST``** **(default)**
     - Counting sort + two-pointer simulation, O(n + m)

.. doxygenfile:: threshold.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: threshold_enum.h
   :project: graph_recognition


References
----------

* V. Chvátal, P. L. Hammer. "Aggregation of inequalities in integer programming."
  *Annals of Discrete Mathematics*, 1:145--162, 1977.
  `DOI:10.1016/S0167-5060(08)70731-3 <https://doi.org/10.1016/S0167-5060(08)70731-3>`_

* N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.*
  Annals of Discrete Mathematics 56, North-Holland, 1995.
