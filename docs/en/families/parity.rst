Parity Graph
============

A **parity graph** is a graph in which every pair of vertices has all
induced paths between them of the same parity (all even-length or all
odd-length).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_75.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ParityAlgorithm``
     - Description
   * - ``DIRECT_CHECK`` **(default)**
     - Directly verifies that all induced paths between each pair of vertices
       have the same parity.

.. doxygenenum:: graph_recognition::ParityAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ParityResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_parity_direct
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_parity
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ParityLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ParityLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_parity_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/parity.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_parity(g);

       std::cout << std::boolalpha << result.is_parity << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/parity_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_parity_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Burlet, J. P. Uhry. "Parity graphs."
  *Annals of Discrete Mathematics*, 21:253--277, 1984.
  `DOI:10.1016/S0304-0208(08)72939-6 <https://doi.org/10.1016/S0304-0208(08)72939-6>`_

* A. Bouchet. "Reducing prime graphs and recognizing circle graphs."
  *Combinatorica*, 7(3):243--254, 1987.
  `DOI:10.1007/BF02579301 <https://doi.org/10.1007/BF02579301>`_
