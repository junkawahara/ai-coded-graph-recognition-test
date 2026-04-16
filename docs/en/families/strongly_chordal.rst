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

.. doxygenfile:: strongly_chordal.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: strongly_chordal_enum.h
   :project: graph_recognition
