Snark
=====

Determines whether a graph is a snark.
A snark is a cubic graph that is bridgeless, has girth at least 5, is
cyclically 4-edge-connected and has chromatic index 4 (that is, it is not
3-edge-colorable).  The smallest snark is the Petersen graph (n = 10), so
every graph on fewer than 10 vertices is rejected.

Recognition
-----------

.. doxygenenum:: graph_recognition::SnarkAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_snark
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SnarkLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_labeled_graphs
   :project: graph_recognition

Both enumerators default to ``SNARKHUNTER``. They construct prime cubic
roots from K4 using Figure 2 of Brinkmann--Goedgebeur--McKay (2011), then
subdivide two distinct edges and connect the new vertices. The first valid
edge reduction in the canonical labeling defines the parent; matching
parent codes and deduplicating sibling codes removes isomorphic copies.
Intermediates remain simple, connected and cubic. Triangles and bridges
must be allowed along construction paths; short-cycle bounds only prune
when the remaining insertions cannot achieve girth 5.

At the final insertion, a 3-edge-coloring supplies even two-colour cycles.
Two edges on the same cycle cannot yield a snark (Brinkmann et al. 2013,
Theorem 3.3). One Kempe exchange supplies additional cycles for this
look-ahead. Surviving graphs still undergo exact coloring and cyclic
edge-connectivity tests. The labeled enumerator expands every representative
into all ``n! / |Aut(G)|`` different labelings.

The previous vertex-addition algorithms remain available as
``SnarkLabeledEnumAlgorithm::REVERSE_SEARCH`` and
``SnarkUnlabeledEnumAlgorithm::CANONICAL_AUGMENTATION``, preserving their
output order and representative labels. The corresponding CLI options are
``--algo reverse_search`` and ``--algo canonical_augmentation``;
``--algo snarkhunter`` explicitly selects the new default.

The new algorithms accept ``0 <= n <= 63``. The legacy labeled algorithm
retains its limit of 30 vertices, and the legacy unlabeled algorithm accepts
up to 63. Odd orders and orders below 10 give no output. There is no
``connected_only`` option because every snark is connected. Enumeration is
available in C++ and the CLI; Python exposes recognition only.

The implementation does not depend on external snarkhunter or nauty. It
uses the shared cubic BFS canonicalization (worst-case exponential), its
own reduction order and sibling sets. Triangle bundling, expansion orbits,
a second independent coloring and the later large-girth algorithms are
not implemented. Prime codes, sibling sets, all outputs and each label
orbit are retained in memory; the original program's speed and polynomial
delay are not claimed.

.. doxygenenum:: graph_recognition::SnarkUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SnarkUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_snark_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

Tests verify the counts ``1, 0, 0, 0, 2`` for ``n = 10, 12, 14, 16, 18``
(`OEIS A130315 <https://oeis.org/A130315>`_), recognizer validity and absence
of isomorphic duplicates. They compare all 30,240 labeled Petersen graphs
with the legacy algorithm and unlabeled classes through 14 vertices.
Independent cubic generation agrees through 12 vertices, the prime counts
agree through 18 vertices, and an independent coloring routine verifies
all look-ahead rejections for cubic parents through 10 vertices. Disabling
the look-ahead preserves the output. A separate 20-vertex run returned six
snarks.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/snark.h"

   int main() {
       using namespace graph_recognition;

       // Petersen graph (the smallest snark)
       Graph g(10, {
           {1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 1},
           {6, 8}, {8, 10}, {10, 7}, {7, 9}, {9, 6},
           {1, 6}, {2, 7}, {3, 8}, {4, 9}, {5, 10}
       });
       auto result = check_snark(g);

       std::cout << std::boolalpha << result.is_snark << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/snark_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_labeled_graphs(10);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/snark_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_snark_unlabeled_graphs(10);
       std::cout << result.graphs.size() << '\n';  // 1 (the Petersen graph)
       return 0;
   }


References
----------

* R. Isaacs. "Infinite families of nontrivial trivalent graphs which are not Tait colorable."
  *The American Mathematical Monthly*, 82(3):221--239, 1975.
  `DOI:10.2307/2319844 <https://doi.org/10.2307/2319844>`_

* J. Petersen. "Die Theorie der regulären Graphs."
  *Acta Mathematica*, 15:193--220, 1891.
  `DOI:10.1007/BF02392606 <https://doi.org/10.1007/BF02392606>`_

* G. Brinkmann, J. Goedgebeur, B. D. McKay. "Generation of Cubic graphs."
  *Discrete Mathematics & Theoretical Computer Science* 13(2):69–80, 2011.
  `Paper <https://dmtcs.episciences.org/551/pdf>`_.
* G. Brinkmann, J. Goedgebeur, J. Hägglund, K. Markström. "Generation and properties of snarks."
  *Journal of Combinatorial Theory, Series B*, 103(4):468--488, 2013.
  `DOI:10.1016/j.jctb.2013.05.001 <https://doi.org/10.1016/j.jctb.2013.05.001>`_

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
