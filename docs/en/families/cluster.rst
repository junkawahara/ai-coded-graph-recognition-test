Cluster Graph
=============

A **cluster graph** is a disjoint union of complete graphs.
Equivalently, it is P3-free (contains no induced path on 3 vertices).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ClusterAlgorithm``
     - Description
   * - **(default)**
     - Default cluster graph recognition algorithm.

Recognition
-----------

.. doxygenfile:: cluster.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cluster_enum.h
   :project: graph_recognition


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
