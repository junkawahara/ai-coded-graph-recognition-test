Unit Interval Graph
===================

Determines whether a graph is a unit interval graph.
An interval graph where all intervals have the same length.
By Roberts' theorem, equivalent to proper interval graphs.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``UnitIntervalAlgorithm``
     - Description
   * - ``PROPER_INTERVAL`` **(default)**
     - Reduces to proper interval graph recognition (Roberts' theorem).

Recognition
-----------

.. doxygenfile:: unit_interval.h
   :project: graph_recognition


References
----------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.
