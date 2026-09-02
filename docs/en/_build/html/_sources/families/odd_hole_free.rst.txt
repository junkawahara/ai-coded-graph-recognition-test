Odd-Hole-Free Graph
===================

An **odd-hole-free graph** contains no induced odd cycle of length five
or more.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_356.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

The recognition function ``check_odd_hole_free`` does not accept an algorithm
parameter. It reuses ``has_odd_hole`` from ``perfect.h``, which detects odd
holes via BFS and DFS on restricted subgraphs for each edge.

.. doxygenstruct:: graph_recognition::OddHoleFreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_odd_hole_free
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::OddHoleFreeLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OddHoleFreeLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_odd_hole_free_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/odd_hole_free.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_odd_hole_free(g);

       std::cout << std::boolalpha << result.is_odd_hole_free << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/odd_hole_free_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_odd_hole_free_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Chudnovsky, A. Scott, P. Seymour, S. Spirkl. "Detecting an odd hole."
  *Journal of the ACM*, 67(1):5:1--5:12, 2020.
  `DOI:10.1145/3375720 <https://doi.org/10.1145/3375720>`_
