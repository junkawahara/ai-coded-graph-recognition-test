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
     - Iterative removal of isolated / universal vertices; every removal
       rescans all vertices, so O(n^2 + m)
   * - ``DEGREE_SEQUENCE_FAST`` **(default)**
     - Counting sort + two-pointer simulation, O(n)

.. doxygenenum:: graph_recognition::ThresholdAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdResult
   :project: graph_recognition
   :members:

``ThresholdResult`` also reports the creation sequence. A threshold graph is
built from one vertex by repeatedly adding an isolated or a dominating
vertex, and reversing the eliminations the recognizers perform gives exactly
that sequence (``creation_order[i]`` is the i-th vertex added,
``creation_kind[i]`` is 0 for isolated and 1 for dominating). It is replayed
against the graph before being returned, so it can be used as a certificate.

.. doxygenfunction:: graph_recognition::check_threshold
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ThresholdUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThresholdUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThresholdUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_threshold_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated non-isomorphic
threshold graphs was verified to match `OEIS A011782
<https://oeis.org/A011782>`_ (which lists the number of threshold graphs
on ``n`` nodes, after Hougardy): ``1, 2, 4, 8, 16, 32``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/threshold.h"

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
   #include "enumerators/threshold_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_threshold_unlabeled_graphs(4);
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
