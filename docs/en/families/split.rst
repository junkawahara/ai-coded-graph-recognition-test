Split Graph
===========

A graph is a **split graph** if its vertex set can be partitioned into a clique
and an independent set.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n^2)
   * - **``HAMMER_SIMEONE``** **(default)**
     - Hammer-Simeone degree-sequence condition, O(n + m)

.. doxygenfile:: split.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: split_enum.h
   :project: graph_recognition
