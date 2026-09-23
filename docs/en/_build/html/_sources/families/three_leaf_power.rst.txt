3-Leaf Power Graph
==================

Determines whether a graph is a 3-leaf power.
By Brandstadt & Le (2006), equivalent to a (bull, dart, gem)-free chordal graph,
which can be recognized by checking that the critical clique graph is a forest.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_651.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_three_leaf_power
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ThreeLeafPowerLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThreeLeafPowerLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search
   :project: graph_recognition

The labeled enumerator defaults to ``VERTEX_INCREMENTAL``: it reuses the
structural unlabeled generator described below, including disconnected
component composition, then expands every representative into all
``n! / |Aut(G)|`` distinct labelings. Adjacent label swaps generate the
labelings, and a visited edge-set collection removes automorphism duplicates,
including twins and identical components. Restricting the additions to
increasing labels would miss graphs whose largest-labeled vertex is neither
pendant nor a twin, such as a P5 with its middle vertex labeled 5.

``REVERSE_SEARCH`` preserves the original chordal-tree search with 3-leaf
power recognition and its output order. Both algorithms emit the same set,
possibly in different orders; the historical C++ function name is retained.
The CLI accepts ``--algo vertex_incremental`` (default) or
``--algo reverse_search``. Python accepts
``enumerate_three_leaf_power_labeled_graphs(n, algorithm="vertex_incremental")``
or ``algorithm="reverse_search"``; omitting ``algorithm`` selects the new default.

The structural method accepts ``0 <= n <= 63`` in C++ and the CLI, inherited
from the unlabeled generator's bitmasks. The legacy method retains
``max_vertex_count``. Both return one empty graph at n = 0; Python keeps its
existing ``1 <= n <= 6`` range. The implementation retains canonical-form
sets, connected representatives by size, visited labelings and all output
graphs in memory. The specialized canonical-parent search and delay bound
of Yamazaki, Qian and Uehara (2024) are not implemented here.

The unlabeled enumerator emits one representative per isomorphism class.
A connected 3-leaf power is exactly a
graph whose critical clique graph is a tree with adjacent cliques joined
completely (Brandstädt & Le 2006) — that is, a tree with a clique
substituted for every vertex. Of the Bandelt & Mulder (1986) one-vertex
extensions, adding a true twin therefore always stays in the class, and
adding a pendant vertex stays in the class exactly when the target vertex
has no true twin or the graph is complete (otherwise its critical clique
splits and the critical clique graph gains a triangle). The false-twin
extension is never needed: a connected member with a non-singleton critical
clique arises by a true-twin extension, and one whose critical cliques are
all singletons is a tree, so it arises by a pendant extension. Generation
proceeds level by level from K1, deduplicating each level by a set of
canonical forms; no recognizer is called. Disconnected members are composed
per integer partition of the component sizes, since the class is closed
under disjoint union. The count is OEIS A277863
(1, 1, 2, 5, 12, 32, 82, 227, 629, ...) with ``connected_only``, and its
Euler transform (1, 2, 4, 10, 24, 65, 171, 478, 1341, ...) without it.

.. doxygenenum:: graph_recognition::ThreeLeafPowerUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ThreeLeafPowerUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ThreeLeafPowerUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_unlabeled_graphs
   :project: graph_recognition

OEIS count check
----------------

For the non-isomorphic enumeration, the ``connected_only`` counts were
checked against `OEIS A277863 <https://oeis.org/A277863>`_
(``1, 1, 2, 5, 12, 32, 82, 227, 629, 1840``) up to ``n = 10``. The overall
counts including disconnected graphs
(``1, 2, 4, 10, 24, 65, 171, 478, 1341``) were checked to coincide with the
set of isomorphism classes obtained by canonicalizing the legacy labeled
enumerator's output up to ``n = 7``. Regression tests compare the complete
labeled sets of both algorithms through ``n = 6`` (8254 graphs).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/three_leaf_power.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_three_leaf_power(g);

       std::cout << std::boolalpha << result.is_three_leaf_power << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/three_leaf_power_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       auto legacy = enumerate_three_leaf_power_labeled_graphs_reverse_search(
           4, ThreeLeafPowerLabeledEnumAlgorithm::REVERSE_SEARCH);
       std::cout << legacy.graphs.size() << '\n';  // Also 61
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/three_leaf_power_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 24
       return 0;
   }


References
----------

* N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees."
  *Journal of Algorithms*, 42(1):69--108, 2002.
  `DOI:10.1006/jagm.2001.1195 <https://doi.org/10.1006/jagm.2001.1195>`_

* A. Brandstädt, V. B. Le. "Structure and linear-time recognition of 3-leaf powers."
  *Information Processing Letters*, 98(4):133--138, 2006.
  `DOI:10.1016/j.ipl.2006.01.004 <https://doi.org/10.1016/j.ipl.2006.01.004>`_

* M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems."
  *Algorithmica*, 44(4):363--381, 2006.
  `DOI:10.1007/s00453-005-1180-z <https://doi.org/10.1007/s00453-005-1180-z>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_

* K. Yamazaki, M. Qian, R. Uehara. "Efficient enumeration of non-isomorphic
  distance-hereditary graphs and related graphs."
  *Discrete Applied Mathematics*, 342:190--199, 2024.
  `DOI:10.1016/j.dam.2023.09.002 <https://doi.org/10.1016/j.dam.2023.09.002>`_
