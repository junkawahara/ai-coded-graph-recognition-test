(k,g)-Graph / Cage
==================

A **(k,g)-graph** is a ``k``-regular graph with girth at least ``g``
(the class GENREG generates; some authors require girth exactly ``g``,
which is ``is_kg_graph && girth == g`` here), and a **(k,g)-cage** is a
``k``-regular graph of girth exactly ``g`` with the minimum possible
number of vertices ``n(k,g)``: the complete graph ``K_{k+1}`` is the
(k,3)-cage, the complete bipartite graph ``K_{k,k}`` the (k,4)-cage,
the cycle ``C_g`` the (2,g)-cage, the Petersen graph the (3,5)-cage,
the Heawood graph the (3,6)-cage and the McGee graph the (3,7)-cage.
Every ``k``-regular graph with girth ``>= g`` (``k >= 2``) has at least
``M(k,g)`` vertices — the **Moore bound**,
``1 + k((k-1)^{(g-1)/2} - 1)/(k-2)`` for odd ``g`` and
``2((k-1)^{g/2} - 1)/(k-2)`` for even ``g``. Like the partial k-trees
and the k-degenerate graphs (and unlike the other classes in this
library), membership is parameterized: ``k`` and ``g`` are inputs to
the recognizer and the enumerator, which is also why this class has no
Python bindings — the extra parameters do not fit the wrapper's
uniform signature.

Recognition
-----------

Regularity and girth are checked directly: the girth by the standard
truncated BFS from every vertex in ``O(nm)``, reported exactly on
every input (0 = acyclic = infinite girth). Cage-ness additionally
needs minimality, for which no polynomial certificate is known; it is
decided *exactly* by running the girth-constrained existence search of
the enumerator on every feasible order from the Moore bound
``M(k,g)`` up to ``n - 1``. Moore-tight cages (``C_g``, ``K_{k+1}``,
``K_{k,k}``, the Petersen and Heawood graphs) are therefore confirmed
with no search at all, while each order above the bound costs an
exhaustive nonexistence proof — exponential in the worst case, which
ends the practical range before the McGee graph (``n = 24``, two
orders above ``M(3,7) = 22``). Minimality is tested against graphs of
girth *at least* ``g``; this equals the standard exact-girth
definition by the strict monotonicity of ``n(k,g)`` in ``g``
(Fu--Huang--Rodger 1997).

.. doxygenenum:: graph_recognition::CageAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CageResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cage
   :project: graph_recognition


Enumeration
-----------

The enumerator emits one representative per isomorphism class of the
``k``-regular graphs with girth ``>= g`` on ``n`` vertices — GENREG's
degree- and girth-constrained orderly generation (Meringer 1999)
realized on the shared canonical-augmentation machinery, i.e. the
k-regular enumerator (its ``g <= 3`` special case) with the girth
constraint folded into the search. Every cycle created by joining the
new vertex to a neighborhood ``S`` passes through the new vertex, so
girth ``>= g`` is preserved exactly when the members of ``S`` are
pairwise at distance ``>= g - 2`` in the current graph (the
generalization of the snark enumerator's ``g = 5`` rule), enforced via
truncated-BFS "far masks" computed once per level; the degree
completability pruning of the k-regular search carries over verbatim,
and ``n`` below the Moore bound returns empty immediately. Isomorph
rejection is McKay's canonical construction path method. The same
search with an early exit is exposed as ``cage_kg_graph_exists`` — the
nonexistence side of a cage-order computation, and what ``check_cage``
uses to certify minimality.

.. doxygenenum:: graph_recognition::CageUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CageUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CageUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cage_unlabeled_graphs
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::cage_kg_graph_exists
   :project: graph_recognition

Count checks
------------

``g <= 3`` places no constraint on a simple graph, so the enumeration
reproduces the k-regular one class for class (rows of the OEIS
triangle A051031). ``k = 2`` counts the partitions of ``n`` into parts
``>= g`` (disjoint unions of cycles of length ``>= g``), and ``k <= 1``
graphs are acyclic, so the girth constraint is vacuous there. (3,5)
first becomes nonempty at ``n = 10`` with exactly the Petersen graph
and counts 1, 2, 9 at ``n = 10, 12, 14``, matching the connected
counts A014372 (no disconnected member exists below ``n = 20``);
(3,6) first becomes nonempty at ``n = 14`` with exactly the Heawood
graph, and ``n = 16`` yields exactly the Möbius--Kantor graph. The
cubic cage orders are A000066. GENREG's throughput does not transfer:
the exact canonicalization branches heavily on sparse high-girth
graphs (locally tree-like, so many orderings tie), bounding the
practical range at about ``n = 14`` for (3,5) and ``n = 16`` for
(3,6).


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cage.h"

   int main() {
       using namespace graph_recognition;

       // The Petersen graph is the (3,5)-cage
       Graph g(10, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
                    {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10},
                    {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6}});
       auto res = check_cage(g, 3, 5);
       std::cout << std::boolalpha
                 << res.is_cage << '\n';      // true
       std::cout << res.girth << '\n';        // 5
       std::cout << check_cage(g, 3, 6).is_kg_graph << '\n';  // false
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cage_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       // The Petersen graph is the only (3,5)-graph on 10 vertices
       auto result = enumerate_cage_unlabeled_graphs(10, 3, 5);
       std::cout << result.graphs.size() << '\n';  // 1
       // ... and none exists on fewer (the cage property)
       std::cout << cage_kg_graph_exists(8, 3, 5) << '\n';  // 0
       return 0;
   }


References
----------

* M. Meringer. "Fast generation of regular graphs and construction of
  cages." *Journal of Graph Theory*, 30(2):137--146, 1999.

* G. Exoo, R. Jajcay. "Dynamic cage survey." *Electronic Journal of
  Combinatorics*, Dynamic Survey DS16, 2008 (revised 2013).

* H.-L. Fu, K.-C. Huang, C. A. Rodger. "Connectivity of cages."
  *Journal of Graph Theory*, 24(2):187--191, 1997.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.

* `OEIS A000066 <https://oeis.org/A000066>`_, `OEIS A051031 <https://oeis.org/A051031>`_, `OEIS A014372 <https://oeis.org/A014372>`_
