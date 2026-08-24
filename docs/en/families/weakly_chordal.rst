Weakly Chordal Graph
====================

A graph is **weakly chordal** if neither the graph nor its complement contains
an induced cycle of length five or more.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_14.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``WeaklyChordalAlgorithm``
     - Description
   * - ``CO_CHORDAL_BIPARTITE``
     - Detects holes (induced cycles of length >= 5) in G and in an explicitly
       built complement (the enum name is historical; no reduction to chordal
       bipartite graphs is involved).
   * - ``COMPLEMENT_BFS`` **(default)**
     - The same hole detection, but antiholes are found by BFS over the
       complement without materializing it.

Both variants are polynomial but well above O(n m): candidate hole edges are
enumerated pairwise and every candidate pair triggers a BFS, which is already
Theta(n^3) on an edgeless graph and O(n^6) in the worst case.

.. doxygenenum:: graph_recognition::WeaklyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::WeaklyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_weakly_chordal_co
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_weakly_chordal_complement_bfs
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_weakly_chordal
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_EDGE_ADDITION`` is Kiyomi's weakly-chordal-subgraph
reverse search. Its root is the empty spanning subgraph of ``K_n``. After
fixing a lexicographic total order on the edges, the parent of a nonempty
weakly chordal graph ``H`` deletes the youngest edge whose deletion leaves a
weakly chordal graph. Hayward's generation theorem guarantees that such an
edge exists. The traversal tries every missing edge ``e`` and accepts
``H + e`` exactly when it is weakly chordal and its unique parent is ``H``.
It therefore searches weakly chordal graphs directly instead of enumerating a
larger graph class and filtering it.

``GENERIC_VERTEX_AUGMENTATION`` retains the former implementation as a
fallback. It adds vertices in label order and tests every candidate
neighborhood with the weakly chordal recognizer. ``REVERSE_SEARCH`` and
``KIYOMI`` are compatibility aliases for the default.

With an O(m²) recognizer, Kiyomi's analysis gives O(m⁴) time per weakly
chordal subgraph of the host graph and O(n + m) space. This implementation
reuses the existing ``check_weakly_chordal`` recognizer, whose documented
worst-case bound is O(n⁶), so the present direct bound for host ``K_n`` is
O(n¹⁰) delay. The callback API uses O(n²) search state; the materializing API
also stores the output.

.. doxygenenum:: graph_recognition::WeaklyChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::WeaklyChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_weakly_chordal_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_weakly_chordal_graphs_reverse_search_cb
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "weakly_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_weakly_chordal(g);

       std::cout << std::boolalpha << result.is_weakly_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "weakly_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_weakly_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms*.
  Ph.D. thesis, The Graduate University for Advanced Studies, 2006,
  Section 4.1.5, Theorem 4.16.
  `PDF <https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf>`_

* R. B. Hayward. "Generating weakly triangulated graphs."
  *Journal of Graph Theory*, 21(1):67--69, 1996.
  `DOI record <https://doi.org/10.1002/(SICI)1097-0118(199601)21:1%3C67::AID-JGT9%3E3.0.CO;2-K>`_

* R. B. Hayward. "Weakly triangulated graphs."
  *Journal of Combinatorial Theory, Series B*, 39(3):200--208, 1985.
  `DOI:10.1016/0095-8956(85)90050-4 <https://doi.org/10.1016/0095-8956(85)90050-4>`_

* J. P. Spinrad, R. Sritharan. "Algorithms for weakly triangulated graphs."
  *Discrete Applied Mathematics*, 59(2):181--191, 1995.
  `DOI:10.1016/0166-218X(93)E0161-Q <https://doi.org/10.1016/0166-218X(93)E0161-Q>`_
