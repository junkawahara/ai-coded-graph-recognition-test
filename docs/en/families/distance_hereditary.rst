Distance-Hereditary Graph
=========================

A graph is **distance-hereditary** if, in every connected induced subgraph,
the distances between vertices are preserved from the original graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_80.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - Description
   * - ``HASHMAP_TWINS``
     - Iterative pendant/twin vertex removal using hash-map twin detection.
   * - ``SORTED_TWINS``
     - Iterative pendant/twin vertex removal using sorted neighbor-list
       comparison.  Deterministic.
   * - ``HASH_TWINS`` **(default)**
     - XOR hash incremental twin detection.  Each vertex receives a random
       64-bit weight; upon removal the neighbor hashes are updated in O(1).
       Hash matches are verified by exact neighbor-list comparison.
       Candidate buckets are rescanned after each removal, so the worst
       case is O(n\ :sup:`3`) on dense graphs.

   * - ``SPLIT_DECOMPOSITION``
     - Checks that every bag of Cunningham's split decomposition is degenerate
       (Bandelt & Mulder 1986; see ``split_decomposition.h`` in
       :doc:`../api/utilities`). Slow, but it shares nothing with the three
       twin-elimination variants, which makes it a genuinely independent
       check.

.. doxygenenum:: graph_recognition::DistanceHereditaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_distance_hereditary
   :project: graph_recognition

Pruning sequence construction
-------------------------------

Distance-hereditary graphs are exactly the graphs that can be reduced to a
single vertex by repeatedly deleting a pendant vertex or one of a pair of
twins (Bandelt & Mulder 1986). ``build_pruning_sequence()`` constructs such a
sequence and names the witness of every step; it is replayed against the
graph before being returned.

``check_distance_hereditary()`` does not build the sequence, so recognition
costs the same as before: recording the witnesses is more work than the fast
recognizers pay to answer the question.

.. doxygenstruct:: graph_recognition::PruningStep
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PruningSequenceResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_pruning_sequence
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::DistanceHereditaryLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search
   :project: graph_recognition

The labeled enumerator defaults to ``VERTEX_INCREMENTAL``. It reuses the
unlabeled generator described below, including disconnected component
composition, and expands each representative into all distinct labelings.
Adjacent label swaps reach all vertex permutations; a visited edge-set
collection removes duplicates caused by twins and interchangeable components.
Each representative contributes exactly ``n! / |Aut(G)|`` labeled graphs. The
largest-labeled vertex need not be pendant or a twin, so restricting extension
order to increasing labels alone would miss graphs.

``REVERSE_SEARCH`` selects the previous largest-label vertex-addition search
with recognition. Both methods produce the same set in different orders;
the historical function name remains unchanged. C++ and the CLI accept
``0 <= n <= 63`` for ``VERTEX_INCREMENTAL`` and ``0 <= n <= 64`` for
``REVERSE_SEARCH`` (bitmask limits), with one empty graph for ``n = 0``.
The CLI selects them with ``--algo vertex_incremental`` (default) or
``--algo reverse_search``. Python's
``enumerate_distance_hereditary_labeled_graphs(n, algorithm=None)`` accepts
``"vertex_incremental"`` and ``"reverse_search"``, within its existing
``1 <= n <= 6`` range. Both return 892 graphs for n = 5.

The default retains connected representatives by size, the current
representative's visited labelings, and all output graphs. It does not
implement the DH-tree parent search or the O(n³) delay for non-isomorphic
graphs with at most n vertices in
`Yamazaki–Qian–Uehara (2024) <https://doi.org/10.1016/j.dam.2023.09.002>`_.

The unlabeled generator emits a single representative per isomorphism
class, from the one-vertex extensions of
Bandelt & Mulder (1986): adding a pendant vertex, a true twin, or a false
twin. The connected members on k vertices are exactly the extensions of the
connected members on k - 1 vertices, so the search grows level by level from
K1 and rejects isomorphs by a canonical-form set per level. No recognizer is
called anywhere, and a graph has 3(k - 1) children instead of the
2\ :sup:`k-1` neighborhoods the legacy labeled reverse search tests. Disconnected
members are composed from the connected ones -- the class is closed under
disjoint union -- as multisets of components over the integer partitions of
n. The counts are OEIS A277862(n) (1, 1, 2, 6, 18, 73, 308, 1484, 7492, ...)
with ``connected_only`` set, or its Euler transform
(1, 2, 4, 11, 31, 114, 454, 2078, 10168, ...) without it.

.. doxygenenum:: graph_recognition::DistanceHereditaryUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DistanceHereditaryUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

With ``connected_only``, the non-isomorphic enumeration was verified through
``n = 9`` against `OEIS A277862 <https://oeis.org/A277862>`_:
``1, 1, 2, 6, 18, 73, 308, 1484, 7492``. The counts including disconnected
members, ``1, 2, 4, 11, 31, 114, 454, 2078, 10168``, were checked through
``n = 7`` against the set of isomorphism classes obtained by canonicalizing
the labeled enumerator's output.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/distance_hereditary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_distance_hereditary(g);

       std::cout << std::boolalpha << result.is_distance_hereditary << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/distance_hereditary_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/distance_hereditary_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 31
       return 0;
   }


References
----------

* E. Howorka. "A characterization of distance-hereditary graphs."
  *The Quarterly Journal of Mathematics*, 28(4):417--420, 1977.
  `DOI:10.1093/qmath/28.4.417 <https://doi.org/10.1093/qmath/28.4.417>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_

* P. L. Hammer, F. Maffray. "Completely separable graphs."
  *Discrete Applied Mathematics*, 27(1--2):85--99, 1990.
  `DOI:10.1016/0166-218X(90)90131-U <https://doi.org/10.1016/0166-218X(90)90131-U>`_
