Proper Interval Graph
=====================

Determines whether a graph is a proper interval graph.
An interval graph representable by a family of intervals with no proper containment.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ProperIntervalAlgorithm``
     - Description
   * - ``PQ_TREE``
     - Interval graph + claw (K_{1,3}) detection via triple loop. Complexity: O(n * Delta^3).
   * - ``FAST_CLAW_CHECK`` **(default)**
     - Interval graph + claw detection via edge counting.
       Only performs detailed search when N(c) has fewer than d(d-1)/2 edges. Complexity: O(m * Delta).

.. doxygenfile:: proper_interval.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: proper_interval_enum.h
   :project: graph_recognition
