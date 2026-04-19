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


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "tournament.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {1, 3}, {2, 3}};
       auto result = check_tournament(3, arcs);

       std::cout << std::boolalpha << result.is_tournament << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tournament_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournaments(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* J. W. Moon. *Topics on Tournaments.*
  Holt, Rinehart and Winston, New York, 1968.
