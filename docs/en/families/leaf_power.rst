Leaf Power Family
=================

A graph G is a k-leaf power if there exists a tree T whose leaves are the vertices of G,
and two vertices are adjacent in G if and only if their distance in T is at most k.


three_leaf_power.h -- 3-Leaf Power Graph
-----------------------------------------

Determines whether a graph is a 3-leaf power.
By Brandstadt & Le (2006), equivalent to a (bull, dart, gem)-free chordal graph,
which can be recognized by checking that the critical clique graph is a forest.

.. doxygenfile:: three_leaf_power.h
   :project: graph_recognition


three_leaf_power_enum.h -- 3-Leaf Power Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: three_leaf_power_enum.h
   :project: graph_recognition


four_leaf_power.h -- 4-Leaf Power Graph
-----------------------------------------

Determines whether a graph is a 4-leaf power.

.. doxygenfile:: four_leaf_power.h
   :project: graph_recognition


four_leaf_power_enum.h -- 4-Leaf Power Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: four_leaf_power_enum.h
   :project: graph_recognition


five_leaf_power.h -- 5-Leaf Power Graph
-----------------------------------------

Determines whether a graph is a 5-leaf power.

.. doxygenfile:: five_leaf_power.h
   :project: graph_recognition


five_leaf_power_enum.h -- 5-Leaf Power Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: five_leaf_power_enum.h
   :project: graph_recognition
