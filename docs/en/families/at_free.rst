AT-Free Graph
=============

A graph is **AT-free** (asteroidal triple-free) if it contains no asteroidal
triple -- three vertices such that each pair is connected by a path that
avoids the closed neighborhood of the third vertex.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ATFreeAlgorithm``
     - Description
   * - ``BRUTE_FORCE`` **(default)**
     - Checks all triples (u, v, w) via BFS to determine whether each pair
       can be connected by a path avoiding the neighborhood of the third.

Recognition
-----------

.. doxygenfile:: at_free.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: at_free_enum.h
   :project: graph_recognition


References
----------

* C. G. Lekkerkerker, J. Ch. Boland. "Representation of a finite graph by a set of intervals on the real line."
  *Fundamenta Mathematicae*, 51(1):45--64, 1962.
  `DOI:10.4064/fm-51-1-45-64 <https://doi.org/10.4064/fm-51-1-45-64>`_

* D. G. Corneil, S. Olariu, L. Stewart. "Asteroidal triple-free graphs."
  *SIAM Journal on Discrete Mathematics*, 10(3):399--430, 1997.
  `DOI:10.1137/S0895480193250125 <https://doi.org/10.1137/S0895480193250125>`_
