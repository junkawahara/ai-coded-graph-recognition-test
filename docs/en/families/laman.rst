Laman Graph
===========

Determines whether a graph is a Laman graph.
A graph satisfying the Laman condition: m = 2n - 3 and every subgraph on k >= 2
vertices has at most 2k - 3 edges (for k = 1 the bound 2k - 3 is negative, so the
subset condition is imposed on subsets of at least two vertices only). The
implementation, ``check_laman``, additionally accepts the single-vertex graph K_1
as Laman, following the OEIS convention and matching the enumerator's output.
Laman graphs characterize minimally rigid frameworks in the plane.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1206.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::LamanAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_laman
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::LamanLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs; the second emits one representative
per isomorphism class. Both default to ``HENNEBERG``. Starting from K2,
Henneberg I attaches a new vertex to any two distinct existing vertices;
Henneberg II subdivides an edge and connects the new vertex to a third
existing vertex. All intermediates are Laman, so this path needs no
recognition or subset-sparsity checks. A canonical-form set per size removes
duplicates across parents and operations, and only the current and next
levels are retained. The labeled version uses adjacent label swaps and an
edge-set visited set to emit all distinct labelings of each representative.
Fixing construction order to label order would omit graphs whose largest
label does not admit an inverse Henneberg move.

The previous algorithms remain selectable as ``REVERSE_SEARCH`` (labeled)
and ``CANONICAL_AUGMENTATION`` (unlabeled), preserving their output order
and representative labels. They grow through (2,3)-sparse intermediates
using incremental subset checks, with tightness enforced at emission.
The unlabeled legacy algorithm also uses McKay's canonical-parent test.
The new and old algorithms can emit different orders and representatives.

Both modes return no graphs at n = 0, K1 at n = 1, and K2 at n = 2.
Laman graphs are connected, so there is no ``connected_only`` flag.
C++/CLI allow up to 63 vertices for Henneberg construction and unlabeled
legacy search, and 64 for labeled ``REVERSE_SEARCH``. There are no Python
enumeration bindings for Laman graphs; Python exposes recognition only.

The CLI uses ``--algo henneberg`` (default), ``--algo reverse_search``
(labeled), or ``--algo canonical_augmentation`` (unlabeled). Each k-vertex
parent has k(k-1)/2 + (2k-3)(k-2) Henneberg candidates, each requiring
the shared worst-case factorial canonicalizer. The APIs retain all outputs;
label expansion also retains visited labelings of the current representative.
No polynomial-delay guarantee is claimed.

.. doxygenenum:: graph_recognition::LamanUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::LamanUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_laman_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

The Henneberg enumeration was verified through ``n = 9`` against
`OEIS A227117 <https://oeis.org/A227117>`_
(``1, 1, 1, 1, 3, 13, 70, 608, 7222`` for ``n = 1, ..., 9``; the
legacy ``n = 10`` output was previously verified as 110132). The new
labeled output equals the legacy output set through ``n = 6`` and the
exhaustive pebble-game oracle at ``n = 6``; unlabeled classes agree with
the legacy search through ``n = 7``. Tests include K3,3 and the triangular
prism, which require Henneberg II, and a labeling whose largest vertex has
degree 5. The static count tests stop at ``n = 9``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/laman.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}});
       auto result = check_laman(g);

       std::cout << std::boolalpha << result.is_laman << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/laman_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/laman_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_laman_unlabeled_graphs(6);
       std::cout << result.graphs.size() << '\n';  // 13
       return 0;
   }


References
----------

* Taufiqurrachman, Y. Yamauchi, S. Kijima, M. Yamashita.
  "Enumeration of Henneberg decompositions of Laman graphs and M-Circuits."
  Sections 2.4 and 3.1.
  `Author manuscript <https://www.ipsj-kyushu.jp/page/ronbun/hinokuni/1001/B-1/B-1-3.pdf>`_

* G. Laman. "On graphs and rigidity of plane skeletal structures."
  *Journal of Engineering Mathematics*, 4(4):331--340, 1970.
  `DOI:10.1007/BF01534980 <https://doi.org/10.1007/BF01534980>`_

* D. J. Jacobs, B. Hendrickson. "An algorithm for two-dimensional rigidity percolation: the pebble game."
  *Journal of Computational Physics*, 137(2):346--365, 1997.
  `DOI:10.1006/jcph.1997.5809 <https://doi.org/10.1006/jcph.1997.5809>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
