Strongly Chordal Graph
======================

A graph is **strongly chordal** if it is chordal and every even cycle of length
six or more has an odd chord (a chord connecting two vertices at odd distance
along the cycle).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - Description
   * - ``STRONG_ELIMINATION``
     - Strong elimination ordering check, O(n^4)
   * - ``PEO_MATRIX``
     - PEO-based matrix method, O(n^2 + n m Delta)
   * - **``MCS_SEO``** **(default)**
     - MCS-based strong elimination ordering, O(n^2 + n m)

Recognition
-----------

.. doxygenfile:: strongly_chordal.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: strongly_chordal_enum.h
   :project: graph_recognition


References
----------

* M. Farber. "Characterizations of strongly chordal graphs."
  *Discrete Mathematics*, 43(2--3):173--189, 1983.
  `DOI:10.1016/0012-365X(83)90129-9 <https://doi.org/10.1016/0012-365X(83)90129-9>`_

* E. Dahlhaus, P. Duchet. "On strongly chordal graphs."
  *Ars Combinatoria*, 24B:23--30, 1987.
