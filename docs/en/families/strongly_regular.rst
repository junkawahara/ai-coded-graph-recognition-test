Strongly Regular Graph
======================

Determines whether a graph is strongly regular with parameters (n, k, lambda, mu).
A k-regular graph where every pair of adjacent vertices has exactly lambda common neighbors
and every pair of non-adjacent vertices has exactly mu common neighbors.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1185.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::StronglyRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_regular
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::StronglyRegularLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyRegularLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_regular_labeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "strongly_regular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       auto result = check_strongly_regular(g);

       std::cout << std::boolalpha << result.is_strongly_regular << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "strongly_regular_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_regular_labeled_graphs(5);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. C. Bose. "Strongly regular graphs, partial geometries and partially balanced designs."
  *Pacific Journal of Mathematics*, 13(2):389--419, 1963.
  `DOI:10.2140/pjm.1963.13.389 <https://doi.org/10.2140/pjm.1963.13.389>`_
