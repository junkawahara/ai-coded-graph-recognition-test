Proper Chordal Graph
====================

A graph is **proper chordal** if it is the intersection graph of a family of
subtrees of a tree such that no subtree is contained in another (the
nesting-free property).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1363.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenstruct:: graph_recognition::ProperChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_proper_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ProperChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search
   :project: graph_recognition


References
----------

* C. Paul, E. Protopapas. "Proper chordal graphs."
  *Proceedings of STACS 2024*, LIPIcs 289, 53:1--53:17, 2024.
  `DOI:10.4230/LIPIcs.STACS.2024.53 <https://doi.org/10.4230/LIPIcs.STACS.2024.53>`_
