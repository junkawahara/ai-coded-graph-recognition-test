Partial k-Tree
==============

A **partial k-tree** is a subgraph of a k-tree, equivalently a graph of
**treewidth at most k**. ``k = 1`` gives the forests, ``k = 2`` the
series-parallel (K4-minor-free) graphs. The class is minor-closed for
every fixed ``k``. Unlike the other classes in this library, membership
is parameterized: ``k`` is an input to the recognizer and the enumerator
(every graph is a partial ``(n-1)``-tree, so nothing can be inferred),
which is also why this class has no Python bindings — the extra
parameter does not fit the wrapper's uniform signature.

Recognition
-----------

Deciding treewidth ``<= k`` is NP-complete when ``k`` is part of the
input (Arnborg--Corneil--Proskurowski 1987), so the recognizer is a
memoized exact search over **elimination orders**: a graph has treewidth
``<= k`` iff its vertices can be eliminated one by one so that each
eliminated vertex has at most ``k`` neighbors in the current *fill
graph* (where two vertices are adjacent iff the graph joins them by a
path whose interior is already eliminated). The search branches on the
vertex eliminated next and memoizes on the set of eliminated vertices —
the fill graph is determined by that set alone, which bounds the search
by the O*(2^n) elimination-order dynamic program of Bodlaender, Fomin,
Koster, Kratsch and Thilikos. Three prunings decide most instances long
before that bound: simplicial vertices of fill degree ``<= k`` are
eliminated without branching (``tw(G) = max(deg(v), tw(G - v))`` for
simplicial ``v``), a simplicial vertex of fill degree ``> k`` kills its
branch (its closed fill neighborhood is a clique on more than ``k + 1``
vertices), and once at most ``k + 1`` vertices remain any completion
works. A degeneracy lower bound (degeneracy ``<=`` treewidth) rejects
many NO instances up front.

YES answers carry a certifying elimination order and its replayed width
(an upper bound on the treewidth, at most ``k``); read backwards the
order is a k-tree construction order of a supergraph.

.. doxygenenum:: graph_recognition::PartialKTreeAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PartialKTreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_partial_ktree
   :project: graph_recognition


Enumeration
-----------

The enumerator emits one representative per isomorphism class of the
graphs of treewidth ``<= k`` on ``n`` vertices — the survey's "geng +
treewidth filter" route realized on the shared canonical-augmentation
machinery. Graphs are grown one vertex at a time; the class is
minor-closed and in particular hereditary, so ``check_partial_ktree``
prunes every candidate child before the far more expensive exact
canonicalization runs. Isomorph rejection is McKay's canonical
construction path method (a child survives iff the newly added vertex
lies in the canonical-deletion orbit reported by
``canonicalize_bitmask_graph``).

.. doxygenenum:: graph_recognition::PartialKTreeUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PartialKTreeUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PartialKTreeUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_partial_ktree_unlabeled_graphs
   :project: graph_recognition

Count checks
------------

``k = 1`` reproduces the unlabeled forests (OEIS A005195: 1, 2, 3, 6,
10, 20, 37, ... ; trees A000055 in connected mode) and ``k = 2`` the
series-parallel graphs (1, 2, 4, 10, 27, 92, 360, 1715, ...), class for
class against the dedicated enumerators. ``k >= n - 1`` yields all
graphs (A000088: 1, 2, 4, 11, 34, 156, ...). The treewidth ``<= 3``
counts (1, 2, 4, 11, 33, 145, 861, 7604 for n = 1..8) are not in the
OEIS; they were verified against a brute-force treewidth filter over all
graphs through ``n = 6``. Practical to about ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/partial_ktree.h"

   int main() {
       using namespace graph_recognition;

       // C5 has treewidth 2
       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       std::cout << std::boolalpha
                 << check_partial_ktree(g, 1).is_partial_ktree << '\n';  // false
       std::cout << check_partial_ktree(g, 2).is_partial_ktree << '\n';  // true
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/partial_ktree_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_partial_ktree_unlabeled_graphs(6, 3);
       std::cout << result.graphs.size() << '\n';  // 145
       return 0;
   }


References
----------

* S. Arnborg, D. G. Corneil, A. Proskurowski. "Complexity of finding
  embeddings in a k-tree." *SIAM Journal on Algebraic and Discrete
  Methods*, 8(2):277--284, 1987.

* H. L. Bodlaender. "A partial k-arboretum of graphs with bounded
  treewidth." *Theoretical Computer Science*, 209(1--2):1--45, 1998.

* H. L. Bodlaender, F. V. Fomin, A. M. C. A. Koster, D. Kratsch,
  D. M. Thilikos. "On exact algorithms for treewidth." *ACM Transactions
  on Algorithms*, 9(1):12:1--12:23, 2012.

* M. J. Dinneen. "Practical enumeration methods for graphs of bounded
  pathwidth and treewidth." CDMTCS Research Report CDMTCS-055,
  University of Auckland, 1997.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A005195 <https://oeis.org/A005195>`_, `OEIS A000055 <https://oeis.org/A000055>`_, `OEIS A000088 <https://oeis.org/A000088>`_
