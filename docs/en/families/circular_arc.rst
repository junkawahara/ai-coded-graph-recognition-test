Circular-Arc Graph
==================

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


Enumeration
-----------

.. doxygenfile:: circular_arc_enum.h
   :project: graph_recognition


References
----------

* A. Tucker. "An efficient test for circular-arc graphs."
  *SIAM Journal on Computing*, 9(1):1--24, 1980.
  `DOI:10.1137/0209001 <https://doi.org/10.1137/0209001>`_

* R. M. McConnell. "Linear-time recognition of circular-arc graphs."
  *Algorithmica*, 37(2):93--147, 2003.
  `DOI:10.1007/s00453-003-1032-7 <https://doi.org/10.1007/s00453-003-1032-7>`_
