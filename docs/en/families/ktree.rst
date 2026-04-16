K-Tree
======

A graph is a **k-tree** if it is chordal, every maximal clique has size exactly
k + 1, and every minimal separator has size exactly k. Equivalently, a k-tree
can be constructed starting from a complete graph on k vertices and repeatedly
adding a new vertex adjacent to exactly k vertices that form a clique.

.. doxygenfile:: ktree.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: ktree_enum.h
   :project: graph_recognition


References
----------

* D. J. Rose. "On simple characterizations of k-trees."
  *Discrete Mathematics*, 7(3--4):317--322, 1974.
  `DOI:10.1016/0012-365X(74)90042-4 <https://doi.org/10.1016/0012-365X(74)90042-4>`_

* L. W. Beineke, R. E. Pippert. "The number of labeled k-dimensional trees."
  *Journal of Combinatorial Theory*, 6(2):200--205, 1969.
  `DOI:10.1016/S0021-9800(69)80120-1 <https://doi.org/10.1016/S0021-9800(69)80120-1>`_
