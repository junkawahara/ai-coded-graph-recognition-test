Trapezoid Graph
===============

Determines whether a graph is a trapezoid graph.
Vertices correspond to trapezoids between two horizontal lines,
with edges between overlapping trapezoids.
Equivalent to being a co-comparability graph whose corresponding
partial order has interval dimension at most 2.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TrapezoidAlgorithm``
     - Description
   * - ``CHAIN_COVER`` **(default)**
     - Based on Cogis (1982) characterization.
       (1) Determines co-comparability via transitive orientation of the complement.
       (2) Constructs the bipartite graph B(P) of the corresponding partial order P
       (edge (x,y) iff NOT x <_P y), then checks bipartiteness of the incompatibility
       graph I(B) (edges from 2+2 patterns) via BFS.
       Trivial 2K_2 (fewer than 4 distinct elements) are excluded.

.. doxygenfile:: trapezoid.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: trapezoid_enum.h
   :project: graph_recognition
