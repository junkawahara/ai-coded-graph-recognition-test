Bipartite Permutation Graph
===========================

Determines whether a graph is a bipartite permutation graph.
A graph that is both bipartite and a permutation graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_81.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``BipartitePermutationAlgorithm``
     - Description
   * - ``CHAIN_BOTH_SIDES`` **(default)**
     - Applies both bipartite recognition and permutation recognition.

.. doxygenenum:: graph_recognition::BipartitePermutationAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_bipartite_permutation
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::BipartitePermutationEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::BipartitePermutationEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_bipartite_permutation_graphs_reverse_search
   :project: graph_recognition


References
----------

* J. P. Spinrad, A. Brandstädt, L. Stewart. "Bipartite permutation graphs."
  *Discrete Applied Mathematics*, 18(3):279--292, 1987.
  `DOI:10.1016/0166-218X(87)90076-0 <https://doi.org/10.1016/0166-218X(87)90076-0>`_

* T. Saitoh, Y. Otachi, K. Yamanaka, R. Uehara. "Random generation and enumeration of bipartite permutation graphs."
  *Journal of Discrete Algorithms*, 10:84--97, 2012.
  `DOI:10.1016/j.jda.2011.12.006 <https://doi.org/10.1016/j.jda.2011.12.006>`_
