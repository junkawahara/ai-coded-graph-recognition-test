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

.. doxygenenum:: graph_recognition::CographAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_cograph
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CographEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CographEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cograph_graphs_cotree
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled cographs was
verified to match the same integer sequence recorded as `OEIS A006351
<https://oeis.org/A006351>`_: ``2, 8, 52, 472, 5504``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cograph.h"

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
   #include "cograph_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cograph_graphs_cotree(4);
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
