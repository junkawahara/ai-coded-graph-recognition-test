Ptolemaic Graph
===============

A graph is **ptolemaic** if it is both chordal and distance-hereditary.
Equivalently, the graph is gem-free and chordal.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_95.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PtolemaicAlgorithm``
     - Description
   * - ``DH_HASHMAP``
     - Distance-hereditary check using hash maps
   * - ``DH_SORTED`` **(default)**
     - Distance-hereditary check using sorted adjacency lists

.. doxygenenum:: graph_recognition::PtolemaicAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_ptolemaic
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PtolemaicEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PtolemaicEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_ptolemaic_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "ptolemaic.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_ptolemaic(g);

       std::cout << std::boolalpha << result.is_ptolemaic << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "ptolemaic_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_ptolemaic_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* E. Howorka. "A characterization of Ptolemaic graphs."
  *Journal of Graph Theory*, 5(3):323--331, 1981.
  `DOI:10.1002/jgt.3190050314 <https://doi.org/10.1002/jgt.3190050314>`_
