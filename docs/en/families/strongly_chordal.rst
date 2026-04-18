Strongly Chordal Graph
======================

A graph is **strongly chordal** if it is chordal and every even cycle of length
six or more has an odd chord (a chord connecting two vertices at odd distance
along the cycle).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_125.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - Description
   * - ``STRONG_ELIMINATION``
     - Strong elimination ordering check, O(n^4)
   * - ``PEO_MATRIX``
     - PEO-based matrix method, O(n^2 + n m Delta)
   * - ``MCS_SEO`` **(default)**
     - MCS-based strong elimination ordering, O(n^2 + n m)

.. doxygenenum:: graph_recognition::StronglyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_chordal_elimination
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_peo_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_mcs_seo
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::StronglyChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_graphs_reverse_search
   :project: graph_recognition


References
----------

* M. Farber. "Characterizations of strongly chordal graphs."
  *Discrete Mathematics*, 43(2--3):173--189, 1983.
  `DOI:10.1016/0012-365X(83)90154-1 <https://doi.org/10.1016/0012-365X(83)90154-1>`_

* E. Dahlhaus, P. Duchet. "On strongly chordal graphs."
  *Ars Combinatoria*, 24B:23--30, 1987.
