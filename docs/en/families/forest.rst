Forest
======

Determines whether a graph is a forest.
An acyclic graph (disjoint union of trees).

Recognition
-----------

.. doxygenenum:: graph_recognition::ForestAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ForestResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_forest
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ForestEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ForestEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ForestEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_forest_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "forest.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_forest(g);

       std::cout << std::boolalpha << result.is_forest << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "forest_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_forest_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
