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
