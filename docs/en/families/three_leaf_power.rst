3-Leaf Power Graph
==================

Determines whether a graph is a 3-leaf power.
By Brandstadt & Le (2006), equivalent to a (bull, dart, gem)-free chordal graph,
which can be recognized by checking that the critical clique graph is a forest.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_651.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_three_leaf_power
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::ThreeLeafPowerEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_three_leaf_power_graphs_reverse_search
   :project: graph_recognition


References
----------

* N. Nishimura, P. Ragde, D. M. Thilikos. "On graph powers for leaf-labeled trees."
  *Journal of Algorithms*, 42(1):69--108, 2002.
  `DOI:10.1006/jagm.2001.1195 <https://doi.org/10.1006/jagm.2001.1195>`_

* A. Brandstädt, V. B. Le. "Structure and linear-time recognition of 3-leaf powers."
  *Information Processing Letters*, 98(4):133--138, 2006.
  `DOI:10.1016/j.ipl.2006.01.004 <https://doi.org/10.1016/j.ipl.2006.01.004>`_

* M. Dom, J. Guo, F. Hüffner, R. Niedermeier. "Error compensation in leaf power problems."
  *Algorithmica*, 44(4):363--381, 2006.
  `DOI:10.1007/s00453-005-1180-z <https://doi.org/10.1007/s00453-005-1180-z>`_
