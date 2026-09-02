Cograph
=======

A graph is a **cograph** if it contains no induced P4 (path on 4 vertices).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_151.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CographAlgorithm``
     - Description
   * - ``COTREE``
     - Cotree construction via recursive connected-component /
       co-component decomposition.
   * - ``PARTITION_REFINEMENT`` **(default)**
     - Same decomposition with a fast linked-list co-component search.
       Complexity: worst case O(n (n + m)) (each decomposition level
       rescans the adjacency lists).

   * - ``MODULAR``
     - Builds the modular decomposition tree and checks it has no PRIME node
       (see ``modular_decomposition.h`` in :doc:`../api/utilities`). A
       cograph's modular decomposition is exactly its cotree, so this reaches
       the same structure by entirely different machinery -- a useful
       independent cross-check, but slower at O(n^4).

.. doxygenenum:: graph_recognition::CographAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cograph
   :project: graph_recognition

Cotree construction
--------------------

The decomposition the recognition algorithms perform already is the cotree.
``build_cotree()`` keeps it instead of discarding it, returning it as an
``MDTree`` (see :doc:`../api/utilities`): a union step becomes a PARALLEL
node, a join step a SERIES node, and no PRIME node ever appears.
``check_cograph()`` does not build the tree, so recognition costs the same as
before.

.. doxygenstruct:: graph_recognition::CotreeResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_cotree
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CographLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_labeled_graphs_cotree
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class by recursive cotree construction
through the complement duality: for n >= 2 complementation swaps the
cotree's root type, so the connected cographs are exactly the complements
of the disconnected ones, which in turn are composed of smaller connected
cographs over integer partitions. No isomorph rejection is needed. The
counts are OEIS A000084(n) (1, 2, 4, 10, 24, 66, 180, 522, ...), or
A000669(n) (1, 1, 2, 5, 12, 33, 90, 261, ...) with ``connected_only``
set.

.. doxygenenum:: graph_recognition::CographUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CographUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled cographs was
verified to match the same integer sequence recorded as `OEIS A006351
<https://oeis.org/A006351>`_: ``2, 8, 52, 472, 5504``. The non-isomorphic
enumeration was verified through ``n = 10`` against `OEIS A000084
<https://oeis.org/A000084>`_ (``1, 2, 4, 10, 24, 66, 180, 522, 1532,
4624``), and against `OEIS A000669 <https://oeis.org/A000669>`_
(``1, 1, 2, 5, 12, 33, 90, 261, 766, 2312``) with ``connected_only``
set.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/cograph.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}});
       auto result = check_cograph(g);

       std::cout << std::boolalpha << result.is_cograph << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/cograph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cograph_labeled_graphs_cotree(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. Seinsche. "On a property of the class of n-colorable graphs."
  *Journal of Combinatorial Theory, Series B*, 16(2):191--193, 1974.
  `DOI:10.1016/0095-8956(74)90063-X <https://doi.org/10.1016/0095-8956(74)90063-X>`_

* D. G. Corneil, H. Lerchs, L. Stewart Burlingham. "Complement reducible graphs."
  *Discrete Applied Mathematics*, 3(3):163--174, 1981.
  `DOI:10.1016/0166-218X(81)90013-5 <https://doi.org/10.1016/0166-218X(81)90013-5>`_

* D. G. Corneil, Y. Perl, L. K. Stewart. "A linear recognition algorithm for cographs."
  *SIAM Journal on Computing*, 14(4):926--934, 1985.
  `DOI:10.1137/0214065 <https://doi.org/10.1137/0214065>`_

* Á. A. Jones, F. Protti, R. R. Del-Vecchio. "Cograph generation with linear delay."
  *Theoretical Computer Science*, 713:1--10, 2018.
  `DOI:10.1016/j.tcs.2017.12.037 <https://doi.org/10.1016/j.tcs.2017.12.037>`_
