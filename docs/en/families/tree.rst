Tree
====

Determines whether a graph is a tree.
A connected acyclic graph, equivalently a connected graph with exactly n-1 edges.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_342.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::TreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tree
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TreeEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tree_graphs
   :project: graph_recognition


References
----------

* A. Cayley. "A theorem on trees."
  *Quarterly Journal of Pure and Applied Mathematics*, 23:376--378, 1889.
