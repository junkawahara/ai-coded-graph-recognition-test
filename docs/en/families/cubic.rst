Cubic Graph
===========

Determines whether a graph is cubic (3-regular).
A graph where every vertex has degree exactly 3.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1100.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CubicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cubic
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CubicEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CubicEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cubic_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cubic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cubic(g);

       std::cout << std::boolalpha << result.is_cubic << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cubic_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cubic_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of cubic graphs."
  *Discrete Mathematics and Theoretical Computer Science*, 13(2):69--80, 2011.
