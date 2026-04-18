Tournament
==========

Determines whether a directed graph is a tournament.
A complete directed graph: for every pair of vertices, exactly one directed arc exists.

Recognition
-----------

.. doxygenenum:: graph_recognition::TournamentAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TournamentResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tournament
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::read_directed
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::TournamentEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournaments
   :project: graph_recognition


References
----------

* J. W. Moon. *Topics on Tournaments.*
  Holt, Rinehart and Winston, New York, 1968.
