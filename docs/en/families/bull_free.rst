Bull-Free Graph
===============

Determines whether a graph is bull-free.
No induced subgraph isomorphic to the bull
(a triangle with two pendant edges at distinct vertices).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_372.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::BullFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BullFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bull_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BullFreeEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BullFreeEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bull_free_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "bull_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_bull_free(g);

       std::cout << std::boolalpha << result.is_bull_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "bull_free_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_bull_free_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Chudnovsky. "The structure of bull-free graphs I --- Three-edge-paths with centers and anticenters."
  *Journal of Combinatorial Theory, Series B*, 102(1):233--251, 2012.
  `DOI:10.1016/j.jctb.2011.07.002 <https://doi.org/10.1016/j.jctb.2011.07.002>`_
