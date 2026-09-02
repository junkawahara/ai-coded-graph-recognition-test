Gem-Free Graph
==============

Determines whether a graph is gem-free.
No induced subgraph isomorphic to the gem
(a fan graph F_{1,4}, i.e., a path P_4 plus a vertex adjacent to all of P_4).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_354.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::GemFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::GemFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_gem_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::GemFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::GemFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_gem_free_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/gem_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_gem_free(g);

       std::cout << std::boolalpha << result.is_gem_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/gem_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_gem_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
