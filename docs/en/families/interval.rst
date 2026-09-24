Interval Graph
==============

Determines whether a graph is an interval graph.
Each vertex corresponds to an interval on the real line,
with edges between overlapping intervals.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_234.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``IntervalAlgorithm``
     - Description
   * - ``BACKTRACKING``
     - After verifying chordality, searches directly over the set of maximal
       cliques for a clique path (an ordering in which the maximal cliques
       containing each vertex are consecutive) by backtracking
       (``search_clique_order`` / ``find_clique_path``). No clique tree is
       built.
   * - ``AT_FREE`` **(default)**
     - Verifies that the graph is chordal and AT-free (asteroidal triple-free),
       based on the Lekkerkerker--Boland theorem.
   * - ``PQ_TREE``
     - Verifies chordality and then tests the consecutive ones property of the
       vertex/maximal-clique incidence matrix with the Booth--Lueker PQ-tree,
       based on the Fulkerson--Gross theorem (see ``pq_tree.h`` in
       :doc:`../api/utilities`).

.. doxygenenum:: graph_recognition::IntervalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_interval
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_KIJIMA_UNO`` algorithm is an interval-specific
edge-deletion reverse search. Its root is ``K_n``. The unique parent adds an
edge between the largest-labeled non-universal vertex and the closest
non-neighbor in an interval model. A one-edge deletion is visited only when
its parent is the current graph. Vertices with larger labels than the pivot
are universal true twins, allowing equivalent deletion candidates to share
one recognition result.

``LEGACY_CHORDAL_FILTER`` selects the former chordal vertex-addition tree with
interval-recognition pruning. ``REVERSE_SEARCH`` is a backward-compatible
alias for ``KIYOMI_KIJIMA_UNO``.

With linear-time interval-model construction, the paper proves ``O(n^3)``
time per output and ``O(n^2)`` space. This implementation reuses the existing
``check_interval`` recognizer for child candidates and constructs a complete
edge list for every callback, so that time bound does not directly apply.

.. doxygenenum:: graph_recognition::IntervalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_interval_labeled_graphs_reverse_search_cb
   :project: graph_recognition

The enumerator above emits labeled graphs. The unlabeled enumerator defaults
to ``MPQ_TREE``, using Mikos's MPQ-tree canonical edge deletion (2021).
It starts at the complete graph and determines deletable edges from MPQ
sections and rotatable paths, constructs the resulting endpoint strings,
removes isomorphic siblings by canonical strings, and verifies their canonical
parent by extending an interval endpoint. No interval recognition or general
graph canonicalization is needed in this backend. ``connected_only`` prunes
branches when a deletion disconnects the graph.

The previous vertex-addition/recognition algorithm remains selectable as
``CANONICAL_AUGMENTATION``. The CLI accepts ``--algo mpq_tree`` (default) or
``--algo canonical_augmentation``; Python accepts the same names in
``algorithm``, after the existing ``connected_only`` argument. The C++/CLI
range remains ``0 <= n <= 63``, and Python accepts ``1 <= n <= 8``. The
legacy option preserves its representative labels and output order.

MPQ construction reuses clique PQ-tree reductions; canonicalization compares
full subtree strings and edge tests compare section sets directly. These
polynomial primitives do not implement the original ``O(n^3 log n)`` delay
bound. The explicit DFS stack and sibling sets use polynomial working space;
all outputs are additionally retained. Counts are OEIS A005975(n)
(1, 2, 4, 10, 27, 92, 369, 1807, 10344, ...), or with ``connected_only``,
A005976 (1, 1, 2, 5, 15, 56, 250, 1328, 8069, ...). At ``n = 0`` both
C++ connectivity modes emit one empty graph.

.. doxygenenum:: graph_recognition::IntervalUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::IntervalUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::IntervalUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_interval_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated labeled interval
graphs was verified to match `OEIS A005215 <https://oeis.org/A005215>`_:
``1, 2, 8, 61, 822, 17914``.

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A005975 <https://oeis.org/A005975>`_
(``1, 2, 4, 10, 27, 92, 369, 1807, 10344``), independently of the
enumerator itself: the same counts come out of filtering the
non-isomorphic chordal enumeration by ``check_interval``. With
``connected_only`` the counts match `OEIS A005976
<https://oeis.org/A005976>`_ (``1, 1, 2, 5, 15, 56, 250, 1328``).
The static test cases stop at ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/interval.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_interval(g);

       std::cout << std::boolalpha << result.is_interval << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/interval_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/interval_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_interval_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 92 = A005975(6)
       return 0;
   }


References
----------

* M. Kiyomi, S. Kijima, T. Uno. "Listing Chordal Graphs and Interval Graphs."
  *Graph-Theoretic Concepts in Computer Science (WG 2006)*,
  LNCS 4271:68--77, 2006.
  `DOI:10.1007/11917496_7 <https://doi.org/10.1007/11917496_7>`_

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* K. S. Booth, G. S. Lueker. "Testing for the consecutive ones property, interval graphs, and graph planarity using PQ-tree algorithms."
  *Journal of Computer and System Sciences*, 13(3):335--379, 1976.
  `DOI:10.1016/S0022-0000(76)80045-1 <https://doi.org/10.1016/S0022-0000(76)80045-1>`_

* D. Corneil, S. Olariu, L. Stewart. "The LBFS structure and recognition of interval graphs."
  *SIAM Journal on Discrete Mathematics*, 23(4):1905--1953, 2009.
  `DOI:10.1137/S0895480100373455 <https://doi.org/10.1137/S0895480100373455>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* K. Yamazaki, T. Saitoh, M. Kiyomi, R. Uehara. "Enumeration of nonisomorphic interval graphs and nonisomorphic permutation graphs."
  *Theoretical Computer Science*, 806:310--322, 2020.
  `DOI:10.1016/j.tcs.2019.04.017 <https://doi.org/10.1016/j.tcs.2019.04.017>`_

* P. Mikos. "Efficient enumeration of non-isomorphic interval graphs."
  *Discrete Mathematics & Theoretical Computer Science*, 23(1), 2021.
  `DOI:10.46298/dmtcs.6164 <https://doi.org/10.46298/dmtcs.6164>`_
