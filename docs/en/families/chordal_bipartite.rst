Chordal Bipartite Graph
=======================

A graph is **chordal bipartite** if it is bipartite and contains no induced
cycle of length six or more.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_79.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Brute-force induced even cycle search
   * - ``BISIMPLICIAL``
     - Bisimplicial edge elimination, O(m n^2)
   * - ``FAST_BISIMPLICIAL`` **(default)**
     - Fast bisimplicial edge elimination, O(m deg^2)

.. doxygenenum:: graph_recognition::ChordalBipartiteAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal_bipartite
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ChordalBipartiteEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalBipartiteEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_bipartite_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal_bipartite.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal_bipartite(g);

       std::cout << std::boolalpha << result.is_chordal_bipartite << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_bipartite_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_bipartite_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs."
  *Journal of Graph Theory*, 2(2):155--163, 1978.
  `DOI:10.1002/jgt.3190020209 <https://doi.org/10.1002/jgt.3190020209>`_

* A. Lubiw. "Doubly lexical orderings of matrices."
  *SIAM Journal on Computing*, 16(5):854--879, 1987.
  `DOI:10.1137/0216057 <https://doi.org/10.1137/0216057>`_
