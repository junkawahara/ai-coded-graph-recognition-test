Threshold Graph
===============

A graph is a **threshold graph** if it can be reduced to the empty graph by
repeatedly removing an isolated vertex or a universal vertex (a vertex adjacent
to all others).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_328.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ThresholdAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Degree-sequence based recognition, O(n m)
   * - ``DEGREE_SEQUENCE_FAST`` **(default)**
     - Counting sort + two-pointer simulation, O(n + m)

.. doxygenenum:: graph_recognition::ThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_threshold
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ThresholdEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThresholdEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_threshold_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated non-isomorphic
threshold graphs was verified to match `OEIS A000079
<https://oeis.org/A000079>`_: ``1, 2, 4, 8, 16, 32``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "threshold.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}});
       auto result = check_threshold(g);

       std::cout << std::boolalpha << result.is_threshold << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "threshold_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_threshold_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* V. Chvátal, P. L. Hammer. "Aggregation of inequalities in integer programming."
  *Annals of Discrete Mathematics*, 1:145--162, 1977.
  `DOI:10.1016/S0167-5060(08)70731-3 <https://doi.org/10.1016/S0167-5060(08)70731-3>`_

* N. V. R. Mahadev, U. N. Peled. *Threshold Graphs and Related Topics.*
  Annals of Discrete Mathematics 56, North-Holland, 1995.
