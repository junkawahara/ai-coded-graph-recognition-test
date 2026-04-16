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
