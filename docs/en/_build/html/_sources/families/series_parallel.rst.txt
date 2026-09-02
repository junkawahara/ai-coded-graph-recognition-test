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

.. doxygenenum:: graph_recognition::SeriesParallelLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_series_parallel_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method — the same scheme as the permutation, circle, chordal, and
outerplanar enumerators. Graphs are grown one vertex at a time, which is
sound because series-parallel graphs are minor-closed, hence hereditary.
The pruning is a ``check_series_parallel`` call (the queue-based
series/parallel reduction) on every candidate child; it runs before the
isomorph rejection, so the more expensive canonicalization only ever sees
series-parallel graphs. The dedicated enumerator of Kawano and Nakano
generates rooted connected series-parallel graphs in constant amortized
time per graph and never canonicalizes; this implementation instead
reuses the shared canonical-augmentation machinery, which is practical to
about ``n = 10``. The counts are 1, 2, 4, 10, 27, 92, 360, 1715, 9356,
... for n = 1, 2, ..., or the connected ones among them
(1, 1, 2, 5, 15, 56, 241, 1245, 7182, ...) with ``connected_only`` set;
neither sequence is in the OEIS as of 2026.

.. doxygenenum:: graph_recognition::SeriesParallelUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SeriesParallelUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SeriesParallelUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_series_parallel_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

The non-isomorphic enumeration was verified through ``n = 8``
independently of the enumerator itself: enumerating **all** unlabeled
graphs by unpruned canonical augmentation and filtering them with the
direct K\ :sub:`4`-minor test of ``util/minor.h`` gives the same counts
(``1, 2, 4, 10, 27, 92, 360, 1715``; connected
``1, 1, 2, 5, 15, 56, 241, 1245``). Neither sequence is in the OEIS
(checked 2026-08). The static test cases stop at ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/series_parallel.h"

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
   #include "enumerators/series_parallel_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_series_parallel_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/series_parallel_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_series_parallel_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 92
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

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* S. Kawano, S.-i. Nakano. "Constant time generation of series-parallel graphs."
  *IEICE Transactions on Fundamentals of Electronics, Communications and Computer Sciences*, E88-A(5):1129--1135, 2005.
  `DOI:10.1093/ietfec/e88-a.5.1129 <https://doi.org/10.1093/ietfec/e88-a.5.1129>`_
