Even-Hole-Free Graph
====================

An **even-hole-free graph** contains no induced even cycle of length four
or more.

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``EvenHoleFreeAlgorithm``
     - Description
   * - **(default)**
     - Default even-hole-free graph recognition algorithm.

Recognition
-----------

.. doxygenfile:: even_hole_free.h
   :project: graph_recognition


Enumeration
-----------

.. doxygenfile:: even_hole_free_enum.h
   :project: graph_recognition


References
----------

* M. Conforti, G. Cornuéjols, A. Kapoor, K. Vušković. "Even-hole-free graphs, Part I: Decomposition theorem."
  *Journal of Graph Theory*, 39(1):6--49, 2002.
  `DOI:10.1002/jgt.10006 <https://doi.org/10.1002/jgt.10006>`_

* M. V. G. da Silva, K. Vušković. "Decomposition of even-hole-free graphs with star cutsets and 2-joins."
  *Journal of Combinatorial Theory, Series B*, 103(1):144--183, 2013.
  `DOI:10.1016/j.jctb.2012.09.001 <https://doi.org/10.1016/j.jctb.2012.09.001>`_
