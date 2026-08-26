Distance-Hereditary Graph
=========================

A graph is **distance-hereditary** if, in every connected induced subgraph,
the distances between vertices are preserved from the original graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_80.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``DistanceHereditaryAlgorithm``
     - Description
   * - ``HASHMAP_TWINS``
     - Iterative pendant/twin vertex removal using hash-map twin detection.
   * - ``SORTED_TWINS``
     - Iterative pendant/twin vertex removal using sorted neighbor-list
       comparison.  Deterministic.
   * - ``HASH_TWINS`` **(default)**
     - XOR hash incremental twin detection.  Each vertex receives a random
       64-bit weight; upon removal the neighbor hashes are updated in O(1).
       Hash matches are verified by exact neighbor-list comparison.
       Candidate buckets are rescanned after each removal, so the worst
       case is O(n\ :sup:`3`) on dense graphs.

   * - ``SPLIT_DECOMPOSITION``
     - Checks that every bag of Cunningham's split decomposition is degenerate
       (Bandelt & Mulder 1986; see ``split_decomposition.h`` in
       :doc:`../api/utilities`). Slow, but it shares nothing with the three
       twin-elimination variants, which makes it a genuinely independent
       check.

.. doxygenenum:: graph_recognition::DistanceHereditaryAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_distance_hereditary
   :project: graph_recognition

Pruning sequence construction
-------------------------------

Distance-hereditary graphs are exactly the graphs that can be reduced to a
single vertex by repeatedly deleting a pendant vertex or one of a pair of
twins (Bandelt & Mulder 1986). ``build_pruning_sequence()`` constructs such a
sequence and names the witness of every step; it is replayed against the
graph before being returned.

``check_distance_hereditary()`` does not build the sequence, so recognition
costs the same as before: recording the witnesses is more work than the fast
recognizers pay to answer the question.

.. doxygenstruct:: graph_recognition::PruningStep
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PruningSequenceResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::build_pruning_sequence
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::DistanceHereditaryLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DistanceHereditaryLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_distance_hereditary_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "distance_hereditary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_distance_hereditary(g);

       std::cout << std::boolalpha << result.is_distance_hereditary << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "distance_hereditary_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_distance_hereditary_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* E. Howorka. "A characterization of distance-hereditary graphs."
  *The Quarterly Journal of Mathematics*, 28(4):417--420, 1977.
  `DOI:10.1093/qmath/28.4.417 <https://doi.org/10.1093/qmath/28.4.417>`_

* H.-J. Bandelt, H. M. Mulder. "Distance-hereditary graphs."
  *Journal of Combinatorial Theory, Series B*, 41(2):182--208, 1986.
  `DOI:10.1016/0095-8956(86)90043-2 <https://doi.org/10.1016/0095-8956(86)90043-2>`_

* P. L. Hammer, F. Maffray. "Completely separable graphs."
  *Discrete Applied Mathematics*, 27(1--2):85--99, 1990.
  `DOI:10.1016/0166-218X(90)90131-U <https://doi.org/10.1016/0166-218X(90)90131-U>`_
