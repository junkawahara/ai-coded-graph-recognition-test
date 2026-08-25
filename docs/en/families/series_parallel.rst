Series-Parallel Graph
=====================

A graph is **series-parallel** if it contains no K\ :sub:`4` minor.
Every series-parallel graph is 2-degenerate, but the converse does not
hold (e.g., a subdivision of K\ :sub:`4` is 2-degenerate yet contains a
K\ :sub:`4` minor).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_275.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SeriesParallelAlgorithm``
     - Description
   * - ``MINOR_CHECK``
     - Full-scan series-parallel reduction (pendant / series / parallel reductions).
   * - ``QUEUE_REDUCTION`` **(default)**
     - Queue-based series-parallel reduction.

.. doxygenenum:: graph_recognition::SeriesParallelAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelResult
   :project: graph_recognition
   :members:

``SeriesParallelResult::reductions`` reports the reduction sequence itself
(``SPReduction::kind`` is 0 for isolated, 1 for pendant, 2 for series -- the
edge u-w was added -- and 3 for parallel, where u-w already existed).
Replaying it removes every vertex, so it can be used as a certificate.

.. doxygenfunction:: graph_recognition::check_series_parallel
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SeriesParallelEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_series_parallel_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "series_parallel.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_series_parallel(g);

       std::cout << std::boolalpha << result.is_series_parallel << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "series_parallel_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_series_parallel_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. J. Duffin. "Topology of series-parallel networks."
  *Journal of Mathematical Analysis and Applications*, 10(2):303--318, 1965.
  `DOI:10.1016/0022-247X(65)90125-3 <https://doi.org/10.1016/0022-247X(65)90125-3>`_

* J. Valdes, R. E. Tarjan, E. L. Lawler. "The recognition of series parallel digraphs."
  *SIAM Journal on Computing*, 11(2):298--313, 1982.
  `DOI:10.1137/0211023 <https://doi.org/10.1137/0211023>`_
