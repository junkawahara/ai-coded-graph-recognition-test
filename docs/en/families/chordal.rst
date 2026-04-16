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
