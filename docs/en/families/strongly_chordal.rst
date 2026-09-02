Strongly Chordal Graph
======================

A graph is **strongly chordal** if it is chordal and every even cycle of length
six or more has an odd chord (a chord connecting two vertices at odd distance
along the cycle).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_125.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``StronglyChordalAlgorithm``
     - Description
   * - ``STRONG_ELIMINATION``
     - Strong elimination ordering check, O(n^4)
   * - ``PEO_MATRIX``
     - Full-scan simple vertex elimination (name is historical; no matrix
       is built), worst case O(n m Delta)
   * - ``MCS_SEO`` **(default)**
     - Simple vertex elimination with degree-sorted inclusion checks,
       worst case O(n m Delta)

   * - ``FARBER_SEO``
     - Farber's partial-order construction, O(n^4). The only variant that
       reports a strong elimination ordering (see ``elimination_orderings.h``
       in :doc:`../api/utilities`); the other three eliminate an arbitrary
       simple vertex, which recognizes the class but does not in general
       produce a strong ordering.

.. doxygenenum:: graph_recognition::StronglyChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_strongly_chordal_elimination
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_peo_matrix
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal_mcs_seo
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_strongly_chordal
   :project: graph_recognition


Enumeration
-----------

The default ``KIYOMI_EDGE_ADDITION`` is the dedicated reverse search from
Kiyomi's strongly chordal subgraph-enumeration algorithm. Its root is the
empty graph and each search step adds one edge. For a nonempty strongly
chordal graph ``H``, take the first non-isolated vertex ``v`` in a strong
elimination ordering and its first neighbor ``w`` in that ordering; ``H-vw``
is the unique parent. By Kiyomi's Lemma 4.11, the same ordering remains a
strong elimination ordering after deleting ``vw``. A candidate ``H+e`` is
visited only if it is strongly chordal and its parent deletes ``e``. The
search therefore never enumerates the larger class of all chordal graphs.

The canonical strong elimination ordering follows Farber's construction. At
elimination step ``i``, strict closed-neighborhood inclusions
``N_i[x] ⊂ N_i[y]`` are accumulated in a partial order, and a simple vertex
that is minimal in that order is selected (with labels breaking ties). Merely
removing arbitrary simple vertices recognizes the class but need not itself
produce a strong elimination ordering.

``LEGACY_CHORDAL_FILTER`` retains the previous chordal vertex-addition tree
and strongly chordal recognition filter for differential testing.
``REVERSE_SEARCH`` is a backward-compatible alias of
``KIYOMI_EDGE_ADDITION``.

The thesis obtains ``O(M min(m log n, n^2))`` time per output and
``O(n+M)`` space, where ``M`` is the host graph's edge count, by constructing
a strong elimination ordering in ``O(min(m log n, n^2))`` time. This
implementation recomputes a straightforward ``O(n^4)`` Farber partial-order
construction for every candidate edge and stores an adjacency matrix and
complete output edge lists, so those bounds do not transfer unchanged. The
callback API avoids retaining all outputs and keeps ``O(n^2)`` search state.

.. doxygenenum:: graph_recognition::StronglyChordalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::StronglyChordalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_strongly_chordal_labeled_graphs_reverse_search_cb
   :project: graph_recognition


Enumeration-count validation
----------------------------

Tests verify the labeled counts ``1, 2, 8, 61, 822, 18034`` for
``n = 1, 2, 3, 4, 5, 6`` and set equality with the legacy algorithm through
the same range. They also compare against recognition-filtered exhaustive
enumeration of every simple graph through ``n = 6``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/strongly_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_strongly_chordal(g);

       std::cout << std::boolalpha << result.is_strongly_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/strongly_chordal_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_strongly_chordal_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Kiyomi. *Studies on Subgraph and Supergraph Enumeration Algorithms*.
  Ph.D. thesis, The Graduate University for Advanced Studies, 2006,
  Section 4.1.3.
  `PDF <https://www.nii.ac.jp/graduate/wp-content/themes/nii_original/assets/pdf/students_thesis/18/kiyomi_Dr_thesis.pdf>`_

* M. Farber. "Characterizations of strongly chordal graphs."
  *Discrete Mathematics*, 43(2--3):173--189, 1983.
  `DOI:10.1016/0012-365X(83)90154-1 <https://doi.org/10.1016/0012-365X(83)90154-1>`_

* E. Dahlhaus, P. Duchet. "On strongly chordal graphs."
  *Ars Combinatoria*, 24B:23--30, 1987.
