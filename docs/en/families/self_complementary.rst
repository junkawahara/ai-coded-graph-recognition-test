Self-Complementary Graph
========================

A **self-complementary graph** is a graph that is isomorphic to its own
complement.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1059.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SelfComplementaryAlgorithm``
     - Description
   * - ``ISOMORPHISM_CHECK`` **(default)**
     - Checks whether the graph is isomorphic to its complement.

.. doxygenenum:: graph_recognition::SelfComplementaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_self_complementary
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::SelfComplementaryEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_graphs
   :project: graph_recognition


References
----------

* H. Sachs. "Über selbstkomplementäre Graphen."
  *Publicationes Mathematicae Debrecen*, 9:270--288, 1962.

* G. Ringel. "Selbstkomplementäre Graphen."
  *Archiv der Mathematik*, 14(1):354--358, 1963.
  `DOI:10.1007/BF01234966 <https://doi.org/10.1007/BF01234966>`_
