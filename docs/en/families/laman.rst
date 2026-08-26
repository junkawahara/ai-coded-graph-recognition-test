Laman Graph
===========

Determines whether a graph is a Laman graph.
A graph satisfying the Laman condition: m = 2n - 3 and every subgraph on k vertices
has at most 2k - 3 edges. Laman graphs characterize minimally rigid frameworks in the plane.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1206.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LamanLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_labeled_graphs
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "laman_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_
