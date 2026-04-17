Cograph
=======

A graph is a **cograph** if it contains no induced P4 (path on 4 vertices).

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
       Complexity: O(n + m).

Recognition
-----------

.. doxygenfile:: cograph.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: cograph_enum.h
   :project: graph_recognition


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
