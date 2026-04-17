Strongly Regular Graph
======================

Determines whether a graph is strongly regular with parameters (n, k, lambda, mu).
A k-regular graph where every pair of adjacent vertices has exactly lambda common neighbors
and every pair of non-adjacent vertices has exactly mu common neighbors.

Recognition
-----------

.. doxygenfile:: strongly_regular.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: strongly_regular_enum.h
   :project: graph_recognition


References
----------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_
