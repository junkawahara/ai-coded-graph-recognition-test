Halin Graph
===========

A **Halin graph** is a planar graph formed by embedding a tree with no
degree-2 vertices in the plane, then connecting all leaf vertices with a cycle.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_198.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::HalinAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::HalinResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_halin
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::HalinEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::HalinEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_halin_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "halin.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_halin(g);

       std::cout << std::boolalpha << result.is_halin << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "halin_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_halin_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. Halin. "Studies on minimally n-connected graphs."
  In D. J. A. Welsh (ed.), *Combinatorial Mathematics and its Applications*,
  Academic Press, pp. 129--136, 1971.
