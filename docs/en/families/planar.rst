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


References
----------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* J. Hopcroft, R. Tarjan. "Efficient planarity testing."
  *Journal of the ACM*, 21(4):549--568, 1974.
  `DOI:10.1145/321850.321852 <https://doi.org/10.1145/321850.321852>`_
