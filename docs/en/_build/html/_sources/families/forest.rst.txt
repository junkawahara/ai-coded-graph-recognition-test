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

.. doxygenenum:: graph_recognition::ForestUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ForestUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ForestUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_forest_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12``, the number of
enumerated non-isomorphic forests was verified to match `OEIS A005195
<https://oeis.org/A005195>`_: ``1, 2, 3, 6, 10, 20, 37, 76, 153, 329,
710, 1601``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/forest.h"

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
   #include "enumerators/forest_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_forest_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
