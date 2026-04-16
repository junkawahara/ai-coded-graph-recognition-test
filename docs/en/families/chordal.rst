Chordal Graph
=============

A graph is **chordal** (triangulated) if it contains no induced cycle of length
four or more. Equivalently, the graph admits a perfect elimination ordering
(PEO).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - Description
   * - ``MCS_PEO``
     - Priority-queue Maximum Cardinality Search, O(n + m log n)
   * - **``BUCKET_MCS_PEO``** **(default)**
     - Bucket-sort Maximum Cardinality Search, O(n + m)
   * - ``LEXBFS_PEO``
     - LexBFS by Rose-Tarjan-Lueker 1976, O(n + m)

.. doxygenfile:: chordal.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: chordal_enum.h
   :project: graph_recognition


References
----------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs."
  *SIAM Journal on Computing*, 5(2):266--283, 1976.
  `DOI:10.1137/0205021 <https://doi.org/10.1137/0205021>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
