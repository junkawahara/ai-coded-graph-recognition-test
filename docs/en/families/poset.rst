Poset (Partially Ordered Set)
=============================

Determines whether a directed graph represents a poset.
A directed acyclic graph representing a partial order
(reflexive, antisymmetric, and transitive relation).

Recognition
-----------

.. doxygenenum:: graph_recognition::PosetAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_poset
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::PosetEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_posets
   :project: graph_recognition


References
----------

* R. P. Dilworth. "A decomposition theorem for partially ordered sets."
  *Annals of Mathematics*, 51(1):161--166, 1950.
  `DOI:10.2307/1969503 <https://doi.org/10.2307/1969503>`_

* G. Brinkmann, B. D. McKay. "Posets on up to 16 Points."
  *Order*, 19(2):147--179, 2002.
  `DOI:10.1023/A:1016543307592 <https://doi.org/10.1023/A:1016543307592>`_
