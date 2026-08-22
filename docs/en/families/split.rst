Split Graph
===========

A graph is a **split graph** if its vertex set can be partitioned into a clique
and an independent set.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_39.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Chordality check of the graph and its complement (the enum name is
       historical; the degree-sequence method is ``HAMMER_SIMEONE``), O(n^2)
   * - ``HAMMER_SIMEONE`` **(default)**
     - Hammer-Simeone degree-sequence condition.  Degrees are read off the
       adjacency lists and sorted by counting sort, so O(n)

.. doxygenenum:: graph_recognition::SplitAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_split
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SplitEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_graphs_reverse_search
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled split graphs
was verified to match `OEIS A179534 <https://oeis.org/A179534>`_:
``2, 8, 58, 632, 9654``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "split.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_split(g);

       std::cout << std::boolalpha << result.is_split << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "split_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* P. L. Hammer, B. Simeone. "The splittance of a graph."
  *Combinatorica*, 1(3):275--284, 1981.
  `DOI:10.1007/BF02579333 <https://doi.org/10.1007/BF02579333>`_

* S. Földes, P. L. Hammer. "Split graphs."
  *Congressus Numerantium*, 19:311--315, 1977.
