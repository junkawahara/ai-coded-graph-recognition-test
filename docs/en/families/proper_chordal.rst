Proper Chordal Graph
====================

A graph is **proper chordal** if it is a chordal graph admitting an
indifference tree-layout (Paul & Protopapas 2024). Note that "subtree
intersection representation with no subtree contained in another" does NOT
define this class: every chordal graph has such a representation (containment
can always be broken by attaching a private new leaf per subtree).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1363.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

``check_proper_chordal`` takes no algorithm parameter.  It builds a block tree
rooted at each vertex (Paul & Protopapas 2024, Algorithm 1) and verifies the
nested-convex condition for every block (Algorithm 2).  The algorithm of the
paper is O(n^4), but the nested-convex verification implemented here enumerates
all vertex orderings of a block, so the worst case is exponential in the size of
the largest block.

.. doxygenstruct:: graph_recognition::ProperChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_proper_chordal
   :project: graph_recognition


Enumeration
-----------

The default enumerator is a proper-chordal-specific edge-addition reverse
search.  Its root is the empty graph.  The recognizer constructs a
deterministic indifference tree-layout ``T(G)``.  The parent of a nonempty
graph ``G`` is ``G-e(G)``, where ``e(G)`` is the lexicographically first graph
edge of maximum tree distance in ``T(G)``.  Such an edge can be deleted while
preserving that layout.  A missing edge is accepted as a child exactly when it
becomes the child's canonical parent edge.  Thus every search node is proper
chordal; the algorithm does not enumerate and filter all chordal graphs.

The source paper does not give an enumeration algorithm.  This reverse search
is derived from its Theorem 6 characterization of indifference tree-layouts.
The former chordal-filter implementation remains available as
``LEGACY_CHORDAL_FILTER`` for differential validation.  Since the current
recognizer brute-forces nested-convex permutations, its implemented delay does
not inherit the theoretical polynomial bound.  The callback API keeps O(n²)
search state without retaining all outputs.

.. doxygenenum:: graph_recognition::ProperChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search_cb
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_chordal(g);

       std::cout << std::boolalpha << result.is_proper_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* C. Paul, E. Protopapas. "Tree-Layout Based Graph Classes: Proper Chordal Graphs."
  *Proceedings of STACS 2024*, LIPIcs 289, 55:1--55:18, 2024.
  `DOI:10.4230/LIPIcs.STACS.2024.55 <https://doi.org/10.4230/LIPIcs.STACS.2024.55>`_
