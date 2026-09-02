AT-Free Graph
=============

A graph is **AT-free** (asteroidal triple-free) if it contains no asteroidal
triple -- three vertices such that each pair is connected by a path that
avoids the closed neighborhood of the third vertex.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_61.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - Description
   * - ``BRUTE_FORCE`` **(default)**
     - Checks all triples (u, v, w) via BFS to determine whether each pair
       can be connected by a path avoiding the closed neighborhood of the third.

.. doxygenenum:: graph_recognition::ATFreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ATFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_at_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ATFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ATFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_at_free_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/at_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_at_free(g);

       std::cout << std::boolalpha << result.is_at_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/at_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_at_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* D. G. Corneil, S. Olariu, L. Stewart. "Asteroidal triple-free graphs."
  *SIAM Journal on Discrete Mathematics*, 10(3):399--430, 1997.
  `DOI:10.1137/S0895480193250125 <https://doi.org/10.1137/S0895480193250125>`_
