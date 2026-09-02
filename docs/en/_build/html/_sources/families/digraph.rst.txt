Directed Graph
==============

Recognition and utilities for general directed graphs.

Recognition
-----------

.. doxygenenum:: graph_recognition::DigraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_digraph
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraphs
   :project: graph_recognition

The enumerator above emits labeled digraphs. A second one emits a single
representative per isomorphism class, by McKay's canonical construction
path method (the nauty suite obtains the same lists via
``geng n | directg``). Digraphs are grown one vertex at a time (deleting
any vertex of a digraph again yields a digraph, so every class is
reachable), each child adds one of the ``4^k`` out/in-neighborhood
subset pairs against the existing vertices, and no recognition filter is
needed because every intermediate digraph is simple by construction.
Isomorph rejection uses a genuinely directed canonical form that packs
two bits per ordered pair (an arc and its reverse), distinguishing
non-adjacency, either single arc, and a bidirectional pair — the
undirected form the tournament enumerator reuses is sound only on a
complete underlying graph. Practical to about ``n = 6`` (1540944
classes, about 7 s). There is no ``connected_only`` flag, matching the
labeled enumerator.

.. doxygenenum:: graph_recognition::DigraphUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraph_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4``, the number of enumerated labeled directed graphs
was verified to match `OEIS A053763 <https://oeis.org/A053763>`_ (whose
comment identifies ``a(n) = 2^(n^2 - n)`` as the number of simple
digraphs without self-loops on ``n`` labeled nodes): ``1, 4, 64, 4096``.

The non-isomorphic enumeration reproduces
`OEIS A000273 <https://oeis.org/A000273>`_ (digraphs on n unlabeled
nodes): ``1, 3, 16, 218, 9608`` for ``n = 1, ..., 5`` in the static test
cases, plus ``1540944`` at ``n = 6`` (about 7 s) checked by hand. For
``n <= 4`` the canonicalized output of the labeled enumerator yields
exactly the same isomorphism classes.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "recognizers/digraph.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 1}, {2, 3}};
       auto result = check_digraph(3, arcs);

       std::cout << std::boolalpha << result.is_digraph << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/digraph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/digraph_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraph_unlabeled_graphs(4);
       std::cout << result.graphs.size() << '\n';  // 218
       return 0;
   }


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
