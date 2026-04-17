Parity Graph
============

A **parity graph** is a graph in which every pair of vertices has all
induced paths between them of the same parity (all even-length or all
odd-length).

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ParityAlgorithm``
     - Description
   * - **(default)**
     - Default parity graph recognition algorithm.

Recognition
-----------

.. doxygenfile:: parity.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: parity_enum.h
   :project: graph_recognition


References
----------

* M. Burlet, J. P. Uhry. "Parity graphs."
  *Annals of Discrete Mathematics*, 21:253--277, 1984.
  `DOI:10.1016/S0304-0208(08)72937-2 <https://doi.org/10.1016/S0304-0208(08)72937-2>`_

* A. Bouchet. "Reducing prime graphs and recognizing circle graphs."
  *Combinatorica*, 7(3):243--254, 1987.
  `DOI:10.1007/BF02579301 <https://doi.org/10.1007/BF02579301>`_
