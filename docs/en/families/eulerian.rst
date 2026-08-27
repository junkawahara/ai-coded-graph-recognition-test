Eulerian Graph
==============

Determines whether a graph is Eulerian.
A graph where every vertex has even degree (connectivity is not required;
a connected Eulerian graph admits an Euler circuit).

Recognition
-----------

.. doxygenenum:: graph_recognition::EulerianAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_eulerian
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::EulerianLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_eulerian_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method with the even-degree constraint. Eulerian graphs are not
hereditary -- deleting a vertex of positive degree leaves its neighbors
with odd degree -- so the intermediate levels of the vertex-by-vertex
search generate all graphs. The constraint instead collapses the last
level: in an Eulerian graph ``G`` the odd-degree vertices of ``G - v``
are exactly the neighbors of ``v``, so every graph on ``n - 1`` vertices
extends to an Eulerian graph in exactly one way (join the new vertex to
the odd-degree vertices, an even-sized set by the handshake lemma), and
the search branches over candidate neighborhoods only below the last
level. A child survives only when the added vertex lies in the
automorphism orbit of the canonically last vertex of the child. The
counts are OEIS A002854(n) (1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120,
...), or the connected ones among them (A003049: 1, 0, 1, 1, 4, 8, 37,
184, ...) with ``connected_only`` set.

.. doxygenenum:: graph_recognition::EulerianUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EulerianUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::EulerianUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_eulerian_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number returned by the Eulerian
enumerator was verified to match the row sums of `OEIS A058878
<https://oeis.org/A058878>`_ (equivalently `OEIS A006125
<https://oeis.org/A006125>`_ with the index shifted by one):
``1, 1, 2, 8, 64, 1024``.

The non-isomorphic enumeration was verified through ``n = 10`` against
`OEIS A002854 <https://oeis.org/A002854>`_
(``1, 1, 2, 3, 7, 16, 54, 243, 2038, 33120``) and cross-checked against
the canonicalized output of the labeled enumerator through ``n = 6``.
With ``connected_only`` the counts match
`OEIS A003049 <https://oeis.org/A003049>`_
(``1, 0, 1, 1, 4, 8, 37, 184``). The static test cases stop at
``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "eulerian.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}});
       auto result = check_eulerian(g);

       std::cout << std::boolalpha << result.is_eulerian << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "eulerian_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_eulerian_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "eulerian_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_eulerian_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 16 = A002854(6)
       return 0;
   }


References
----------

* L. Euler. "Solutio problematis ad geometriam situs pertinentis."
  *Commentarii Academiae Scientiarum Petropolitanae*, 8:128--140, 1741
  (presented 1736).

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
