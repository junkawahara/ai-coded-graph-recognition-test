Apollonian Network
==================

An **Apollonian network** (planar 3-tree) is built from a triangle by
repeatedly picking a (triangular) face and inserting a new vertex joined
to its three corners. Equivalent characterizations: planar 3-trees
(``n >= 4``), stacked triangulations (graphs of stack polytopes),
uniquely 4-colorable planar graphs, and **chordal maximal planar
graphs**. An Apollonian network on ``n >= 3`` vertices has exactly
``3n - 6`` edges.

Recognition
-----------

Recognition combines maximal planarity and chordality: every Apollonian
network is chordal maximal planar by induction over the insertion
sequence, and conversely a simplicial vertex of a chordal maximal planar
graph on ``n >= 5`` vertices has degree exactly 3 (a simplicial vertex of
degree >= 4 would embed K5 into a planar graph), so deleting it leaves a
chordal maximal planar graph again and induction reconstructs the
insertion sequence in reverse.

.. doxygenenum:: graph_recognition::ApollonianAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApollonianResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_apollonian
   :project: graph_recognition


Enumeration
-----------

The enumerator emits one representative per isomorphism class. It grows
from K3 by joining a new vertex to each triangle of the current graph.
Along the construction ``m = 3k - 6`` and chordality (the new vertex is
simplicial) hold automatically, so a single ``check_planar`` call per
candidate decides class membership — insertion into a face keeps
planarity, insertion into a separating triangle never does.

Isomorph rejection is McKay's canonical construction path method, with
the deletion rule restricted to **degree-3 vertices** since the class is
not hereditary (the link of a degree-3 vertex is a triangle, so deleting
it stays in the class, and one always exists for ``n >= 4``). The
canonical parent deletes the vertex at the *last canonical position of
degree 3*: position degrees are reconstructible from the canonical form,
and ``canonicalize_bitmask_graph_orbits``
(util/canonical_augmentation.h) reports the automorphism orbit of every
position, so a child survives iff the newly inserted vertex lies in that
position's orbit. The canonicalized object is the **complement**: the
class has no K5, so the complement's lex-min row search ties out after
four positions, about 100x faster than canonicalizing the graph itself
(whose large independent sets create huge zero-row ties).

The counts are 1 for ``n <= 3`` (K0, K1, K2, K3) and OEIS A027610(n-3)
from there (1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002, ... for
n = 4, 5, ...). Note this is not A007173, which keeps mirror images
distinct — graph isomorphism includes reflections. Practical to about
``n = 13`` (11,002 classes, about 5 minutes; n = 12 about 8 seconds).
Every Apollonian network is connected, so there is no ``connected_only``
flag.

.. doxygenenum:: graph_recognition::ApollonianUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApollonianUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ApollonianUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_apollonian_unlabeled_graphs
   :project: graph_recognition

Count checks
------------

The non-isomorphic enumeration reproduces OEIS A027610(n-3) up to
``n = 13`` (1, 1, 1, 3, 7, 24, 93, 434, 2110, 11002 for n = 4..13). For
``n <= 6`` the class set coincides with the canonicalized
recognizer-accepted edge subsets of ``K_n`` of size ``3n - 6``, and for
``n <= 8`` with the chordality-filtered output of the non-isomorphic
maximal planar enumerator. Static test cases run to ``n = 11``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "apollonian.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_apollonian(g);

       std::cout << std::boolalpha << result.is_apollonian << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "apollonian_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_apollonian_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 7
       return 0;
   }


References
----------

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* F. Hering, R. Hoede, H. Fleischner. "The enumeration of stack polytopes
  and simplicial clusters." *Discrete Mathematics*, 40(2--3):203--217, 1982.

* L. W. Beineke, R. E. Pippert. "Enumerating dissectable polyhedra by
  their automorphism groups." *Canadian Journal of Mathematics*,
  26(1):50--67, 1974.

* `OEIS A027610 <https://oeis.org/A027610>`_, `OEIS A001764 <https://oeis.org/A001764>`_
