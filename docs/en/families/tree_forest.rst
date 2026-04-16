Tree / Forest Family
====================

Connected acyclic graphs (trees) and their generalizations.


tree.h -- Tree
---------------

Determines whether a graph is a tree.
A connected acyclic graph, equivalently a connected graph with exactly n-1 edges.

.. doxygenfile:: tree.h
   :project: graph_recognition


tree_enum.h -- Tree Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: tree_enum.h
   :project: graph_recognition


forest.h -- Forest
-------------------

Determines whether a graph is a forest.
An acyclic graph (disjoint union of trees).

.. doxygenfile:: forest.h
   :project: graph_recognition


forest_enum.h -- Forest Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: forest_enum.h
   :project: graph_recognition


caterpillar.h -- Caterpillar Graph
-----------------------------------

Determines whether a graph is a caterpillar.
A tree where all vertices are within distance 1 of a central path (the spine).
Equivalently, removing all leaves yields a path (or the empty graph).

.. doxygenfile:: caterpillar.h
   :project: graph_recognition


caterpillar_enum.h -- Caterpillar Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: caterpillar_enum.h
   :project: graph_recognition


unicyclic.h -- Unicyclic Graph
-------------------------------

Determines whether a graph is unicyclic.
A connected graph with exactly one cycle, equivalently a connected graph
with exactly n edges.

.. doxygenfile:: unicyclic.h
   :project: graph_recognition


unicyclic_enum.h -- Unicyclic Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: unicyclic_enum.h
   :project: graph_recognition
