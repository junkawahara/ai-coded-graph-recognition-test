Co-Interval Graph
=================

Determines whether a graph is a co-interval graph.
A graph whose complement is an interval graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_157.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoIntervalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies interval graph recognition.

.. doxygenenum:: graph_recognition::CoIntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_interval
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_KIJIMA_UNO`` algorithm complements every labeled interval
graph from the dedicated Kiyomi--Kijima--Uno enumeration. Complementation is a
bijection on the same vertex labels, so no additional recognition or duplicate
elimination is needed. Each interval callback is converted immediately; only
the co-interval results are retained. The complement adds ``O(n²)`` time per
output. The interval paper's per-output time bound does not carry over to this
implementation.

Select ``CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH`` for the original
largest-label vertex-addition search with co-interval recognition.
Both algorithms preserve the existing range ``0 <= n <= 64`` and emit one
empty graph for ``n = 0``. They produce the same set in different orders.

The CLI accepts ``bin/co_interval_labeled_enum --algo kiyomi_kijima_uno``
(default) or ``--algo reverse_search``. In Python, use
``enumerate_co_interval_labeled_graphs(n, algorithm="kiyomi_kijima_uno")`` or
``algorithm="reverse_search"``.

.. doxygenenum:: graph_recognition::CoIntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. The other enumerator emits one
representative per isomorphism class: it enumerates the non-isomorphic
interval graphs (McKay canonical construction path) and outputs the
complement of each. Complementation commutes with relabeling, so it is a
bijection on isomorphism classes, and the counts equal the interval counts.

.. doxygenstruct:: graph_recognition::CoIntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CoIntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled co-interval
graphs was verified, via the complement bijection with interval graphs, to match
`OEIS A005215 <https://oeis.org/A005215>`_: ``1, 2, 8, 61, 822``.

For the non-isomorphic enumeration, the counts were checked against
`OEIS A005975 <https://oeis.org/A005975>`_ (the interval counts;
``1, 2, 4, 10, 27, 92, 369, 1807``) up to ``n = 8``, and the output was
checked to coincide with the set of isomorphism classes obtained by
canonicalizing the labeled enumerator's output up to ``n = 6``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/co_interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_interval(g);

       std::cout << std::boolalpha << result.is_co_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       auto legacy = enumerate_co_interval_labeled_graphs_reverse_search(
           4, CoIntervalLabeledEnumAlgorithm::REVERSE_SEARCH);
       std::cout << legacy.graphs.size() << '\n';  // Also 61 graphs
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/co_interval_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_interval_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 27
       return 0;
   }


References
----------

* M. Kiyomi, S. Kijima, T. Uno. "Listing Chordal Graphs and Interval Graphs."
  *WG 2006*, LNCS 4271, 68--77, 2006.
  `DOI:10.1007/11917496_7 <https://doi.org/10.1007/11917496_7>`_
* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_
