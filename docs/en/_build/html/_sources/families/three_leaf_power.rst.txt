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

.. doxygenstruct:: graph_recognition::ThreeLeafPowerLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_labeled_graphs_reverse_search
   :project: graph_recognition

The enumerator above emits labeled graphs. The other enumerator emits one
representative per isomorphism class. A connected 3-leaf power is exactly a
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
set of isomorphism classes obtained by canonicalizing the labeled
enumerator's output up to ``n = 7``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "three_leaf_power.h"

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
   #include "three_leaf_power_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_three_leaf_power_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "three_leaf_power_unlabeled_enum.h"

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
