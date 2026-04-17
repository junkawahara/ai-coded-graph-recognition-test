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

Recognition
-----------

.. doxygenfile:: trapezoid.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: trapezoid_enum.h
   :project: graph_recognition


References
----------

* I. Dagan, M. C. Golumbic, R. Y. Pinter. "Trapezoid graphs and their coloring."
  *Discrete Applied Mathematics*, 21(1):35--46, 1988.
  `DOI:10.1016/0166-218X(88)90032-3 <https://doi.org/10.1016/0166-218X(88)90032-3>`_

* O. Cogis. "On the Ferrers dimension of a digraph."
  *Discrete Mathematics*, 38(1):47--52, 1982.
  `DOI:10.1016/0012-365X(82)90168-5 <https://doi.org/10.1016/0012-365X(82)90168-5>`_

* T.-H. Ma, J. P. Spinrad. "On the 2-chain subgraph cover and related problems."
  *Journal of Algorithms*, 17(2):251--268, 1994.
  `DOI:10.1006/jagm.1994.1039 <https://doi.org/10.1006/jagm.1994.1039>`_
