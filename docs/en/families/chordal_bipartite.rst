Chordal Bipartite Graph
=======================

A graph is **chordal bipartite** if it is bipartite and contains no induced
cycle of length six or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalBipartiteAlgorithm``
     - Description
   * - ``CYCLE_CHECK``
     - Brute-force induced even cycle search
   * - ``BISIMPLICIAL``
     - Bisimplicial edge elimination, O(m n^2)
   * - **``FAST_BISIMPLICIAL``** **(default)**
     - Fast bisimplicial edge elimination, O(m deg^2)

.. doxygenfile:: chordal_bipartite.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: chordal_bipartite_enum.h
   :project: graph_recognition


References
----------

* M. C. Golumbic, C. F. Goss. "Perfect elimination and chordal bipartite graphs."
  *Journal of Graph Theory*, 2(2):155--163, 1978.
  `DOI:10.1002/jgt.3190020209 <https://doi.org/10.1002/jgt.3190020209>`_

* A. Lubiw. "Doubly lexical orderings of matrices."
  *SIAM Journal on Computing*, 16(5):854--879, 1987.
  `DOI:10.1137/0216057 <https://doi.org/10.1137/0216057>`_
