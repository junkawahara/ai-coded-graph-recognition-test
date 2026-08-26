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

.. doxygenenum:: graph_recognition::CoIntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoIntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_interval_labeled_graphs_reverse_search
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled co-interval
graphs was verified, via the complement bijection with interval graphs, to match
`OEIS A005215 <https://oeis.org/A005215>`_: ``1, 2, 8, 61, 822``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "co_interval.h"

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
   #include "co_interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_
