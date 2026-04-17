Weakly Chordal Graph
====================

A graph is **weakly chordal** if neither the graph nor its complement contains
an induced cycle of length five or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - Description
   * - ``CO_CHORDAL_BIPARTITE``
     - Co-chordal-bipartite based recognition, O(n^2 + n m)
   * - **``COMPLEMENT_BFS``** **(default)**
     - Complement BFS approach, O(n m)

Recognition
-----------

.. doxygenfile:: weakly_chordal.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: weakly_chordal_enum.h
   :project: graph_recognition


References
----------

* R. B. Hayward. "Weakly triangulated graphs."
  *Journal of Combinatorial Theory, Series B*, 39(3):200--208, 1985.
  `DOI:10.1016/0095-8956(85)90050-4 <https://doi.org/10.1016/0095-8956(85)90050-4>`_

* J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs."
  *Discrete Applied Mathematics*, 59(2):181--191, 1995.
  `DOI:10.1016/0166-218X(93)E0161-Q <https://doi.org/10.1016/0166-218X(93)E0161-Q>`_
