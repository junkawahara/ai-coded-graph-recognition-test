Ptolemaic Graph
===============

A graph is **ptolemaic** if it is both chordal and distance-hereditary.
Equivalently, the graph is gem-free and chordal.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - Description
   * - ``DH_HASHMAP``
     - Distance-hereditary check using hash maps
   * - **``DH_SORTED``** **(default)**
     - Distance-hereditary check using sorted adjacency lists

.. doxygenfile:: ptolemaic.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: ptolemaic_enum.h
   :project: graph_recognition


References
----------

* E. Howorka. "A characterization of Ptolemaic graphs."
  *Journal of Graph Theory*, 5(3):323--331, 1981.
  `DOI:10.1002/jgt.3190050314 <https://doi.org/10.1002/jgt.3190050314>`_
