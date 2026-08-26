P5-Free Graph
=============

Determines whether a graph is P_5-free.
No induced path on 5 vertices.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_396.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::P5FreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::P5FreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_p5_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::P5FreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::P5FreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_p5_free_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "p5_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_p5_free(g);

       std::cout << std::boolalpha << result.is_p5_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "p5_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_p5_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
