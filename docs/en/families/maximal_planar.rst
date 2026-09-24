Maximal Planar Graph
====================

A **maximal planar graph** (triangulation) is a planar graph where adding any
edge would violate planarity. In a maximal planar graph, every face
(including the outer face) is a triangle.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_981.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_maximal_planar
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::MaximalPlanarLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_labeled_graphs_reverse_search
   :project: graph_recognition

Both enumerators default to ``PLANTRI``: E3/E4/E5 local expansions from
K4 preserve a cyclic-order embedding of a simple triangulation. Directed-edge
BFS in both orientations gives canonical codes, identifying mirror images.
A canonical inverse expansion chooses one parent class, and sibling codes
remove repeated expansions. No planarity recognizer or edge-count filter
is needed. The labeled enumerator expands each representative into all
``n! / |Aut(G)|`` distinct labelings.

The legacy methods remain available as
``MaximalPlanarLabeledEnumAlgorithm::REVERSE_SEARCH`` and
``MaximalPlanarUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION``, including
their previous output order and representative labels. CLI selection uses
``--algo plantri`` (default), ``--algo reverse_search`` (labeled), or
``--algo canonical_augmentation`` (unlabeled). The Python unlabeled API
accepts ``algorithm="plantri"`` or ``algorithm="canonical_augmentation"``.

C++/CLI retain their limits of 64 labeled / 63 unlabeled vertices and emit
one K_n for n = 0..3; Python retains 1..8. Nonempty outputs are connected,
so there is no ``connected_only`` option. The new method uses O(k) patches
per k-vertex parent and O(k²) work per candidate. Rotation copies and sibling
sets along the DFS path use O(n³) space, plus retained output and the current
representative's visited labelings. This implementation does not depend on
the external plantri program or implement its in-place updates and orbit
pruning; its throughput and a polynomial delay bound are not claimed.

.. doxygenenum:: graph_recognition::MaximalPlanarUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::MaximalPlanarUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_maximal_planar_unlabeled_graphs
   :project: graph_recognition

Count Check
-----------

Published counts match through n = 12 (7,595 classes), including the
icosahedron, which requires E5. At small sizes, complete output sets agree
with both legacy enumerators. Tests also check face walks, inverse local
operations, and canonicalization under relabeling and reflection.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/maximal_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_maximal_planar(g);

       std::cout << std::boolalpha << result.is_maximal_planar << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/maximal_planar_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/maximal_planar_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_maximal_planar_unlabeled_graphs(8);
       std::cout << result.graphs.size() << '\n';  // 14
       return 0;
   }


References
----------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* G. Brinkmann, B. D. McKay. "Fast generation of planar graphs."
  *MATCH Communications in Mathematical and in Computer Chemistry*, 58(2):323--357, 2007.
