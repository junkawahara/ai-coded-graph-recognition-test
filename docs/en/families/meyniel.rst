Meyniel Graph
=============

A **Meyniel graph** is a graph in which every odd cycle of length five or
more has at least two chords.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_194.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``MeynielAlgorithm``
     - Description
   * - ``DIRECT_CHECK`` **(default)**
     - Directly verifies that every odd induced cycle of length five or more
       has at least two chords.

.. doxygenenum:: graph_recognition::MeynielAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MeynielResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_meyniel_direct
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::check_meyniel
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::MeynielEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MeynielEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_meyniel_graphs_reverse_search
   :project: graph_recognition


References
----------

* H. Meyniel. "On the perfect graph conjecture."
  *Discrete Mathematics*, 16(4):339--342, 1976.
  `DOI:10.1016/S0012-365X(76)80008-8 <https://doi.org/10.1016/S0012-365X(76)80008-8>`_

* M. Burlet, J. Fonlupt. "Polynomial algorithm to recognize a Meyniel graph."
  *Annals of Discrete Mathematics*, 21:225--252, 1984.
  `DOI:10.1016/S0304-0208(08)72938-4 <https://doi.org/10.1016/S0304-0208(08)72938-4>`_

* F. Roussel, I. Rusu. "An O(m|m|) algorithm for recognizing Meyniel graphs."
  *Discrete Mathematics*, 235(1--3):279--285, 2001.
  `DOI:10.1016/S0012-365X(00)00282-3 <https://doi.org/10.1016/S0012-365X(00)00282-3>`_
