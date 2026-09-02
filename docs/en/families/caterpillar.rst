Caterpillar Graph
=================

Determines whether a graph is a caterpillar.
A tree where all vertices are within distance 1 of a central path (the spine).
Equivalently, removing all leaves yields a path (or the empty graph).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_784.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::CaterpillarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_caterpillar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CaterpillarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CaterpillarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CaterpillarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_caterpillar_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 3, 4, 5, 6, 7, 8, 9, 10``, the number of enumerated
non-isomorphic caterpillar trees was verified to match `OEIS A005418
<https://oeis.org/A005418>`_ with the index shifted by two:
``1, 2, 3, 6, 10, 20, 36, 72``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/caterpillar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_caterpillar(g);

       std::cout << std::boolalpha << result.is_caterpillar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/caterpillar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_caterpillar_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, A. J. Schwenk. "The number of caterpillars."
  *Discrete Mathematics*, 6(4):359--365, 1973.
  `DOI:10.1016/0012-365X(73)90067-8 <https://doi.org/10.1016/0012-365X(73)90067-8>`_
