Directed Graph Family
=====================

Recognition and enumeration of directed graph classes.
These classes use directed edges (arcs) rather than undirected edges.

.. note::

   Input format for directed graphs uses arc lists instead of edge lists.
   Each line ``u v`` represents a directed arc from u to v.


tournament.h -- Tournament
---------------------------

Determines whether a directed graph is a tournament.
A complete directed graph: for every pair of vertices, exactly one directed arc exists.

.. doxygenfile:: tournament.h
   :project: graph_recognition


tournament_enum.h -- Tournament Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: tournament_enum.h
   :project: graph_recognition


digraph.h -- Directed Graph
-----------------------------

Recognition and utilities for general directed graphs.

.. doxygenfile:: digraph.h
   :project: graph_recognition


digraph_enum.h -- Directed Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: digraph_enum.h
   :project: graph_recognition


poset.h -- Poset (Partially Ordered Set)
------------------------------------------

Determines whether a directed graph represents a poset.
A directed acyclic graph representing a partial order
(reflexive, antisymmetric, and transitive relation).

.. doxygenfile:: poset.h
   :project: graph_recognition


poset_enum.h -- Poset Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: poset_enum.h
   :project: graph_recognition
