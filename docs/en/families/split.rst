Split Graph
===========

A graph is a **split graph** if its vertex set can be partitioned into a clique
and an independent set.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_39.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``SplitAlgorithm``
     - Description
   * - ``DEGREE_SEQUENCE``
     - Chordality check of the graph and its complement (the enum name is
       historical; the degree-sequence method is ``HAMMER_SIMEONE``), O(n^2)
   * - ``HAMMER_SIMEONE`` **(default)**
     - Hammer-Simeone degree-sequence condition.  Degrees are read off the
       adjacency lists and sorted by counting sort, so O(n)

.. doxygenenum:: graph_recognition::SplitAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitResult
   :project: graph_recognition
   :members:

``SplitResult::side`` reports the partition itself (1 = clique side K,
2 = independent side S). Hammer & Simeone (1981) show that when the degree
condition holds, the vertices of the largest degrees are the clique side. The
partition is verified before being returned, so it can be used as a
certificate.

.. doxygenfunction:: graph_recognition::check_split
   :project: graph_recognition


Enumeration
-----------

The default ``KS_PARTITION_CANONICAL`` algorithm uses split-graph structure
directly.  It chooses a partition ``V = K ∪ S``, makes ``K`` a clique and
``S`` a stable set, and gives every ``k ∈ K`` a nonempty neighborhood in
``S``.  This enumerates exactly the S-max partitions, so every candidate is a
split graph and no recognition filter is used.

When a graph has several S-max partitions, its swing vertices form a clique
``A``.  The algorithm accepts only the partition whose swing vertex ``a`` on
the ``S`` side has the smallest label in ``A``.  Hence every labeled graph is
emitted exactly once.  ``LEGACY_CHORDAL_FILTER`` retains the former chordal
vertex-addition search plus split recognition for differential validation.

.. doxygenenum:: graph_recognition::SplitLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SplitLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_split_labeled_graphs_reverse_search
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::enumerate_split_labeled_graphs_reverse_search_cb
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6, 7, 8``, the number of enumerated labeled split graphs
was verified to match `OEIS A179534 <https://oeis.org/A179534>`_:
``2, 8, 58, 632, 9654, 202484, 5843954``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "split.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_split(g);

       std::cout << std::boolalpha << result.is_split << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "split_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_split_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* P. L. Hammer, B. Simeone. "The splittance of a graph."
  *Combinatorica*, 1(3):275--284, 1981.
  `DOI:10.1007/BF02579333 <https://doi.org/10.1007/BF02579333>`_

* S. Földes, P. L. Hammer. "Split graphs."
  *Congressus Numerantium*, 19:311--315, 1977.

* C. Cheng, K. L. Collins, A. N. Trenk. "Split graphs and
  Nordhaus--Gaddum graphs." *Discrete Mathematics*, 339(9):2345--2356, 2016.
  `DOI:10.1016/j.disc.2016.04.001 <https://doi.org/10.1016/j.disc.2016.04.001>`_

* J. M. Troyka. "Split graphs: combinatorial species and asymptotics."
  *Electronic Journal of Combinatorics*, 26(2):P2.42, 2019.
  `arXiv:1803.07248 <https://arxiv.org/abs/1803.07248>`_
