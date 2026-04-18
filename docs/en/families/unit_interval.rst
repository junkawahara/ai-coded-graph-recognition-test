Unit Interval Graph
===================

Determines whether a graph is a unit interval graph.
An interval graph where all intervals have the same length.
By Roberts' theorem, equivalent to proper interval graphs.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_299.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``UnitIntervalAlgorithm``
     - Description
   * - ``PROPER_INTERVAL`` **(default)**
     - Reduces to proper interval graph recognition (Roberts' theorem).

.. doxygenenum:: graph_recognition::UnitIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnitIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unit_interval
   :project: graph_recognition


References
----------

* F. S. Roberts. "Indifference graphs."
  In F. Harary (ed.), *Proof Techniques in Graph Theory*, Academic Press, pp. 139--146, 1969.
