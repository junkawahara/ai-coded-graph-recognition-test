Polyhedral Graph
================

A **polyhedral graph** is a 3-connected planar graph. By Steinitz's theorem,
these are exactly the graphs of convex polyhedra.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_986.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::PolyhedralAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_polyhedral
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::PolyhedralLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_labeled_graphs
   :project: graph_recognition

Both enumerators default to ``PLANTRI``. They reuse the embedded E3/E4/E5
triangulations from maximal planar enumeration as roots and delete edges
incident with triangles (Brinkmann--McKay, section 4). A merged-face
intersection test preserves 3-connectivity: every other face may share
at most one vertex or one boundary edge with the merged face. No planarity
or triconnectivity recognizer is called in this search.

The first missing ear diagonal on the canonical embedding defines a parent
by adding one edge. Repeated parent steps reach a triangulation. Parent
comparisons and sibling codes remove duplicates across deletion paths and
triangulation roots. Directed-edge BFS in both orientations identifies
mirror images; uniqueness of 3-connected planar embeddings then identifies
abstract graph isomorphism classes. The labeled enumerator expands each
representative into all ``n! / |Aut(G)|`` distinct labelings.

Select ``PolyhedralLabeledEnumAlgorithm::REVERSE_SEARCH`` or
``PolyhedralUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION`` for the original
planar vertex-addition searches with final polyhedral recognition. These
modes preserve the previous output order and representative labels.
CLI selection is ``--algo plantri`` (default), ``--algo reverse_search``
(labeled), or ``--algo canonical_augmentation`` (unlabeled).
Python's existing ``enumerate_polyhedral_unlabeled_graphs(n,
algorithm="plantri")`` also accepts ``algorithm="canonical_augmentation"``.

C++/CLI bounds remain 0..64 labeled and 0..63 unlabeled; Python retains
1..8. Sizes below four yield nothing. Every output is 3-connected, so
there is no ``connected_only`` option. Beyond generating triangulations,
the edge search considers O(n) candidates per output, costing O(n²) per
candidate and O(n³) search storage. All output graphs and the current
label-expansion orbit are retained separately. The implementation has no
external plantri dependency and does not claim that program's throughput,
in-place updates, or orbit-pruning optimizations.

.. doxygenenum:: graph_recognition::PolyhedralUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PolyhedralUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_polyhedral_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 4, 5, 6``, the number of enumerated labeled polyhedral graphs
was verified to match `OEIS A096330 <https://oeis.org/A096330>`_:
``1, 25, 1227``.

The non-isomorphic enumeration reproduces
`OEIS A000944 <https://oeis.org/A000944>`_ through ``n = 10``
(32,300 classes). Tests compare the dedicated and legacy output sets through
``n = 6`` labeled and ``n = 7`` unlabeled. Every edge deletion on all classes
through eight vertices is checked against the existing vertex-pair-removal
3-connectivity recognizer, including deletions rejected despite preserving
minimum degree three. The static test cases stop at ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/polyhedral.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_polyhedral(g);

       std::cout << std::boolalpha << result.is_polyhedral << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/polyhedral_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_polyhedral_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


References
----------

* E. Steinitz. "Polyeder und Raumeinteilungen."
  *Encyclopädie der mathematischen Wissenschaften*, Band 3, Heft 9, 1922.

* B. Grünbaum. *Convex Polytopes.*
  Graduate Texts in Mathematics 221, Springer, 2nd edition, 2003.
  `DOI:10.1007/978-1-4613-0019-9 <https://doi.org/10.1007/978-1-4613-0019-9>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
