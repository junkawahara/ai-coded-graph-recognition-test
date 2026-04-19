K-Regular Graph
===============

Determines whether a graph is k-regular.
A graph where every vertex has the same degree k.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1149.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::KRegularAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_kregular
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::KRegularEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::KRegularEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_kregular_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "kregular.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_kregular(g);

       std::cout << std::boolalpha << result.is_kregular << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "kregular_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_kregular_graphs_reverse_search(4, 2);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Brandstädt, V. B. Le, J. P. Spinrad. *Graph Classes: A Survey.*
  SIAM Monographs on Discrete Mathematics and Applications, 1999.
  `DOI:10.1137/1.9780898719796 <https://doi.org/10.1137/1.9780898719796>`_
