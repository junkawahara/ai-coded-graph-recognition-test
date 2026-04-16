Interval Graph
==============

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


Enumeration
-----------

.. doxygenfile:: interval_enum.h
   :project: graph_recognition
