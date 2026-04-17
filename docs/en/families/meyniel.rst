Meyniel Graph
=============

A **Meyniel graph** is a graph in which every odd cycle of length five or
more has at least two chords.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``MeynielAlgorithm``
     - Description
   * - **(default)**
     - Default Meyniel graph recognition algorithm.

Recognition
-----------

.. doxygenfile:: meyniel.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: meyniel_enum.h
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
