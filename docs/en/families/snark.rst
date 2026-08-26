Snark
=====

Determines whether a graph is a snark.
A snark is a cubic graph that is bridgeless, has girth at least 5, is
cyclically 4-edge-connected and has chromatic index 4 (that is, it is not
3-edge-colorable).  The smallest snark is the Petersen graph (n = 10), so
every graph on fewer than 10 vertices is rejected.

Recognition
-----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SnarkLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_labeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "snark.h"

   int main() {
       using namespace graph_recognition;

       // Petersen graph (the smallest snark)
       Graph g(10, {
           {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
           {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6},
           {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}
       });
       auto result = check_snark(g);

       std::cout << std::boolalpha << result.is_snark << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "snark_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_labeled_graphs(10);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_
