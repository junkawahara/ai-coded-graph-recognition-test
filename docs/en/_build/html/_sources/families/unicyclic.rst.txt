Unicyclic Graph
===============

Determines whether a graph is unicyclic.
A connected graph with exactly one cycle, equivalently a connected graph
with exactly n edges.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1202.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::UnicyclicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::UnicyclicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_unicyclic
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::UnicyclicUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::UnicyclicUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_unicyclic_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 3, 4, 5, 6, 7, 8, 9, 10``, the number of enumerated
non-isomorphic connected unicyclic graphs was verified to match `OEIS A001429
<https://oeis.org/A001429>`_: ``1, 2, 5, 13, 33, 89, 240, 657``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/unicyclic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_unicyclic(g);

       std::cout << std::boolalpha << result.is_unicyclic << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/unicyclic_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_unicyclic_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
