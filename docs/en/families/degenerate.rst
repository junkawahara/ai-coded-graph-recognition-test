k-Degenerate
============

A graph is **k-degenerate** if every nonempty induced subgraph has a
vertex of degree at most ``k`` (Lick--White 1970); equivalently, the
vertices can be removed one by one so that each removed vertex has at
most ``k`` neighbors among those remaining. The smallest such ``k`` is
the **degeneracy**. ``k = 1`` gives the forests; outerplanar graphs are
2-degenerate, planar graphs 5-degenerate, and treewidth ``<= k``
implies k-degeneracy. The class is hereditary for every fixed ``k``.
Like the partial k-trees (and unlike the other classes in this
library), membership is parameterized: ``k`` is an input to the
recognizer and the enumerator (every graph is ``(n-1)``-degenerate, so
nothing can be inferred), which is also why this class has no Python
bindings — the extra parameter does not fit the wrapper's uniform
signature.

Recognition
-----------

The recognizer is the classic **minimum-degree peeling** (the
smallest-last ordering of Matula--Beck 1983) with a bucket queue:
repeatedly remove a vertex of minimum degree in the remaining graph.
The maximum degree seen at a removal is exactly the degeneracy, in
``O(n + m)`` time, so the exact degeneracy comes free and is reported
on both YES and NO answers. On YES the peeling order certifies the
answer (every vertex has at most ``k`` neighbors among those not yet
removed); on NO the **(k+1)-core** — the unique maximal induced
subgraph of minimum degree ``>= k + 1``, obtained by exhaustively
deleting vertices of degree ``<= k`` — is a nonempty induced subgraph
in which every vertex has more than ``k`` neighbors, certifying that no
removal order exists.

.. doxygenenum:: graph_recognition::DegenerateAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DegenerateResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_degenerate
   :project: graph_recognition


Enumeration
-----------

The enumerator emits one representative per isomorphism class of the
k-degenerate graphs on ``n`` vertices — the survey's "geng + degeneracy
filter" route realized on the shared canonical-augmentation machinery.
Graphs are grown one vertex at a time; the class is hereditary, so the
``O(n + m)`` ``check_degenerate`` peeling prunes every candidate child
before the far more expensive exact canonicalization runs. Isomorph
rejection is McKay's canonical construction path method (a child
survives iff the newly added vertex lies in the canonical-deletion
orbit reported by ``canonicalize_bitmask_graph``). The dedicated
algorithm of Bauer--Krug--Wagner (ANALCO 2010) counts and generates
*labeled* k-degenerate graphs via well-orderings and is not used by
this non-isomorphic route.

.. doxygenenum:: graph_recognition::DegenerateUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DegenerateUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DegenerateUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_degenerate_unlabeled_graphs
   :project: graph_recognition

Count checks
------------

``k = 1`` reproduces the unlabeled forests (OEIS A005195: 1, 2, 3, 6,
10, 20, 37, ... ; trees A000055 in connected mode), class for class
against the dedicated enumerator. ``k = 0`` yields only the edgeless
graph and ``k >= n - 1`` all graphs (A000088: 1, 2, 4, 11, 34, 156,
...). The 2-degenerate counts (1, 2, 4, 10, 28, 105, 508, 3454, 31935
for n = 1..9), the connected 2-degenerate counts (1, 1, 2, 5, 16, 68,
375, 2822, ...) and the 3-degenerate counts (1, 2, 4, 11, 33, 148,
950, ... for n = 1..7) are not in the OEIS; they were verified against
a brute-force degeneracy filter over all graphs through ``n = 5``.
Since treewidth ``<= k`` implies k-degeneracy, the output properly
contains the partial k-tree classes from ``n = 5, k = 2`` on (28 vs 27
classes: K4 with one edge subdivided is 2-degenerate but has a K4
minor, hence treewidth 3). Practical to about ``n = 8``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/degenerate.h"

   int main() {
       using namespace graph_recognition;

       // C5 has degeneracy 2
       Graph g(5, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1}});
       std::cout << std::boolalpha
                 << check_degenerate(g, 1).is_degenerate << '\n';  // false
       std::cout << check_degenerate(g, 2).is_degenerate << '\n';  // true
       std::cout << check_degenerate(g, 1).degeneracy << '\n';     // 2
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/degenerate_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_degenerate_unlabeled_graphs(6, 2);
       std::cout << result.graphs.size() << '\n';  // 105
       return 0;
   }


References
----------

* D. R. Lick, A. T. White. "k-degenerate graphs." *Canadian Journal of
  Mathematics*, 22(5):1082--1096, 1970.

* D. W. Matula, L. L. Beck. "Smallest-last ordering and clustering and
  graph coloring algorithms." *Journal of the ACM*, 30(3):417--427,
  1983.

* S. B. Seidman. "Network structure and minimum degree." *Social
  Networks*, 5(3):269--287, 1983.

* R. Bauer, M. Krug, D. Wagner. "Enumerating and generating labeled
  k-degenerate graphs." *Proceedings of ANALCO 2010*, pp. 90--98, 2010.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A005195 <https://oeis.org/A005195>`_, `OEIS A000055 <https://oeis.org/A000055>`_, `OEIS A000088 <https://oeis.org/A000088>`_
