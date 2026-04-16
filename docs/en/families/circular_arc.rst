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
