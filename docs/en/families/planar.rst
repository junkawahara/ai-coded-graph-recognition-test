.. _planar-family:

Planar Family
=============

Recognition and enumeration algorithms for planar graphs and related subclasses.


planar.h -- Planar Graph
------------------------

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
^^^^^^^^^^^

.. doxygenfile:: planar_enum.h
   :project: graph_recognition


outer_planar.h -- Outerplanar Graph
------------------------------------

A graph is **outerplanar** if it can be embedded in the plane with all vertices
on the outer face. Equivalently, it contains no K\ :sub:`4` or K\ :sub:`2,3` minor.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OuterPlanarAlgorithm``
     - Description
   * - ``AUGMENTED_PLANARITY`` **(default)**
     - Tests planarity of the augmented graph G + K\ :sub:`1` (a new vertex adjacent to all others). G is outerplanar if and only if G + K\ :sub:`1` is planar.
   * - ``MINOR_CHECK``
     - Edge bound precheck (2n - 3), then backtracking search for K\ :sub:`4` and K\ :sub:`2,3` minors.

.. doxygenfile:: outer_planar.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: outer_planar_enum.h
   :project: graph_recognition


cactus.h -- Cactus Graph
-------------------------

A graph is a **cactus** if every biconnected component is either a single edge
or a simple cycle. Equivalently, any two simple cycles share at most one vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CactusAlgorithm``
     - Description
   * - ``DFS`` **(default)**
     - Biconnected component decomposition via DFS. Each component is verified to be a single edge or a simple cycle. Complexity: O(n + m).

.. doxygenfile:: cactus.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: cactus_enum.h
   :project: graph_recognition


series_parallel.h -- Series-Parallel Graph
-------------------------------------------

A graph is **series-parallel** if it contains no K\ :sub:`4` minor.
Equivalently, it is a 2-degenerate graph.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SeriesParallelAlgorithm``
     - Description
   * - ``MINOR_CHECK``
     - Iterative removal of vertices with degree at most 2 via full scan. Complexity: O(n\ :sup:`2`).
   * - ``QUEUE_REDUCTION`` **(default)**
     - Queue-based degree-2 vertex removal (2-degeneracy test). Complexity: O(n + m).

.. doxygenfile:: series_parallel.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: series_parallel_enum.h
   :project: graph_recognition


apex.h -- Apex Graph
---------------------

A graph is an **apex graph** if removing a single vertex makes it planar.
The algorithm tests planarity of G - v for each vertex v.

.. doxygenfile:: apex.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: apex_enum.h
   :project: graph_recognition


maximal_planar.h -- Maximal Planar Graph
-----------------------------------------

A **maximal planar graph** (triangulation) is a planar graph where adding any
edge would violate planarity. In a maximal planar graph, every face
(including the outer face) is a triangle.

.. doxygenfile:: maximal_planar.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: maximal_planar_enum.h
   :project: graph_recognition


cubic_planar.h -- Cubic Planar Graph
--------------------------------------

A **cubic planar graph** is a planar graph where every vertex has degree
exactly 3.

.. doxygenfile:: cubic_planar.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: cubic_planar_enum.h
   :project: graph_recognition


polyhedral.h -- Polyhedral Graph
---------------------------------

A **polyhedral graph** is a 3-connected planar graph. By Steinitz's theorem,
these are exactly the graphs of convex polyhedra.

.. doxygenfile:: polyhedral.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: polyhedral_enum.h
   :project: graph_recognition


simple_quadrangulation.h -- Simple Quadrangulation
---------------------------------------------------

A **simple quadrangulation** is a 2-connected planar graph where every face
(including the outer face) is a quadrilateral (4-cycle).

.. doxygenfile:: simple_quadrangulation.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: simple_quadrangulation_enum.h
   :project: graph_recognition


halin.h -- Halin Graph
-----------------------

A **Halin graph** is a planar graph formed by embedding a tree with no
degree-2 vertices in the plane, then connecting all leaf vertices with a cycle.

.. doxygenfile:: halin.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: halin_enum.h
   :project: graph_recognition


fullerene.h -- Fullerene Graph
-------------------------------

A **fullerene graph** is a 3-connected cubic planar graph where every face is
either a pentagon or a hexagon. By Euler's formula, every fullerene has exactly
12 pentagonal faces.

.. doxygenfile:: fullerene.h
   :project: graph_recognition


Enumeration
^^^^^^^^^^^

.. doxygenfile:: fullerene_enum.h
   :project: graph_recognition
