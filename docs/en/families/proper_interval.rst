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


References
----------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_
