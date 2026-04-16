Interval / Arc Family
=====================

Intersection graphs of geometric objects on a line or circle.
Interval graphs are intersection graphs of intervals on a line;
circular-arc graphs generalize this to arcs on a circle.


interval.h -- Interval Graph
-----------------------------

Determines whether a graph is an interval graph.
Each vertex corresponds to an interval on the real line,
with edges between overlapping intervals.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - Description
   * - ``BACKTRACKING``
     - After verifying chordality, searches for a clique path on the clique tree
       via backtracking (each vertex's cliques must form a connected subtree).
   * - ``AT_FREE`` **(default)**
     - Verifies that the graph is chordal and AT-free (asteroidal triple-free),
       based on the Lekkerkerker--Boland theorem.

.. doxygenfile:: interval.h
   :project: graph_recognition


interval_enum.h -- Interval Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: interval_enum.h
   :project: graph_recognition


proper_interval.h -- Proper Interval Graph
-------------------------------------------

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


proper_interval_enum.h -- Proper Interval Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: proper_interval_enum.h
   :project: graph_recognition


unit_interval.h -- Unit Interval Graph
---------------------------------------

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

.. doxygenfile:: unit_interval.h
   :project: graph_recognition


co_interval.h -- Co-Interval Graph
------------------------------------

Determines whether a graph is a co-interval graph.
A graph whose complement is an interval graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies interval graph recognition.

.. doxygenfile:: co_interval.h
   :project: graph_recognition


co_interval_enum.h -- Co-Interval Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: co_interval_enum.h
   :project: graph_recognition


circular_arc.h -- Circular-Arc Graph
--------------------------------------

Determines whether a graph is a circular-arc graph.
Each vertex corresponds to an arc on a circle,
with edges between overlapping arcs.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircularArcAlgorithm``
     - Description
   * - ``MCCONNELL`` **(default)**
     - Searches for an endpoint ordering on the circle via backtracking,
       determining each vertex's arc direction via 2-SAT.

.. doxygenfile:: circular_arc.h
   :project: graph_recognition


circular_arc_enum.h -- Circular-Arc Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: circular_arc_enum.h
   :project: graph_recognition


proper_circular_arc.h -- Proper Circular-Arc Graph
----------------------------------------------------

Determines whether a graph is a proper circular-arc graph.
A circular-arc graph representable by arcs with no proper containment.

.. doxygenfile:: proper_circular_arc.h
   :project: graph_recognition


proper_circular_arc_enum.h -- Proper Circular-Arc Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: proper_circular_arc_enum.h
   :project: graph_recognition
