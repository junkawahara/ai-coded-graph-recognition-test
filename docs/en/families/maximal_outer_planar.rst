Maximal Outerplanar Graph
=========================

A **maximal outerplanar graph** is an outerplanar graph where adding any
edge would violate outerplanarity. For ``n >= 3`` these are exactly the
triangulations of a convex polygon: 2-connected, with a unique Hamiltonian
outer cycle and every inner face a triangle, and they coincide with the
simple 2-trees. A maximal outerplanar graph on ``n >= 2`` vertices has
exactly ``2n - 3`` edges.

Recognition
-----------

Recognition combines the outerplanarity check with the edge count
criterion: a simple outerplanar graph on ``n >= 2`` vertices has at most
``2n - 3`` edges, so an outerplanar graph reaching that bound cannot
accept another edge, and conversely every maximal outerplanar graph
attains it.

.. doxygenenum:: graph_recognition::MaximalOuterPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_outer_planar
   :project: graph_recognition


Enumeration
-----------

The enumerator emits a single representative per isomorphism class, by
the dedicated polygon-triangulation construction rather than the generic
canonical-augmentation route. A maximal outerplanar graph on ``n >= 3``
vertices is a triangulation of a convex ``n``-gon, and since a
2-connected outerplanar graph has a *unique* Hamiltonian cycle, every
isomorphism between two such graphs maps outer cycle to outer cycle and
is therefore a symmetry of the polygon. Isomorphism classes thus
correspond bijectively to orbits of the ``Catalan(n-2)`` polygon
triangulations under the dihedral group ``D_n``, so the enumerator walks
the standard apex-per-chord recursion over triangulations and keeps a
diagonal set iff it is the lexicographic minimum among its ``2n``
dihedral images — no recognizer call and no general-graph
canonicalization anywhere. The counts are 1 for ``n = 0, 1, 2``
(K0, K1 and K2) and OEIS A000207 (1, 1, 1, 3, 4, 12, 27, 82, 228,
733, ... for n = 3, 4, ...) from there; the enumeration is practical to
about ``n = 16`` (2,674,440 triangulations, 83,898 classes, about 2 s).
Every maximal outerplanar graph is connected, so there is no
``connected_only`` flag.

.. doxygenenum:: graph_recognition::MaximalOuterPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalOuterPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_outer_planar_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

The non-isomorphic enumeration reproduces OEIS A000207 through
``n = 16`` (1, 1, 1, 3, 4, 12, 27, 82, 228, 733, 2282, 7528, 24834,
83898 for n = 3..16), and for ``n <= 6`` a brute force over all edge
subsets of ``K_n`` of size ``2n - 3`` accepted by the recognizer,
canonicalized, yields exactly the same isomorphism classes (there is no
labeled enumerator for this class to cross-check against). The static
test cases stop at ``n = 12``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "maximal_outer_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {1, 4}, {1, 3}});
       auto result = check_maximal_outer_planar(g);

       std::cout << std::boolalpha << result.is_maximal_outer_planar << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "maximal_outer_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_outer_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 12
       return 0;
   }


References
----------

* M. Bodirsky, É. Fusy, M. Kang, S. Vigerske. "Enumeration and asymptotic
  properties of unlabeled outerplanar graphs."
  *Electronic Journal of Combinatorics*, 14(1):R66, 2007.

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.

* `OEIS A000207 <https://oeis.org/A000207>`_
