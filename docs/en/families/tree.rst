Tree
====

Determines whether a graph is a tree.
A connected acyclic graph, equivalently a connected graph with exactly n-1 edges.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_342.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::TreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tree
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::TreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tree_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12``, the number of
enumerated non-isomorphic trees was verified to match `OEIS A000055
<https://oeis.org/A000055>`_: ``1, 1, 1, 2, 3, 6, 11, 23, 47, 106, 235,
551``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/tree.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_tree(g);

       std::cout << std::boolalpha << result.is_tree << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/tree_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tree_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* A. Cayley. "A theorem on trees."
  *Quarterly Journal of Pure and Applied Mathematics*, 23:376--378, 1889.
