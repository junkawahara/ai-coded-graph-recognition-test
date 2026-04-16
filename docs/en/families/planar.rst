Planar Graph
============

A graph is **planar** if it can be drawn in the plane without edge crossings.
By Kuratowski's theorem, a graph is planar if and only if it contains no
K\ :sub:`5` or K\ :sub:`3,3` minor.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PlanarAlgorithm``
     - Description
   * - ``MINOR_CHECK`` **(default)**
     - Edge bound precheck (3n - 6), then backtracking search for K\ :sub:`5` and K\ :sub:`3,3` minors.

.. doxygenfile:: planar.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: planar_enum.h
   :project: graph_recognition
