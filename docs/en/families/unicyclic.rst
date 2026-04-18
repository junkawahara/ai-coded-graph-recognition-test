Unicyclic Graph
===============

Determines whether a graph is unicyclic.
A connected graph with exactly one cycle, equivalently a connected graph
with exactly n edges.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1202.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::UnicyclicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnicyclicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unicyclic
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::UnicyclicEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::UnicyclicEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_unicyclic_graphs
   :project: graph_recognition


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
