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

.. doxygenenum:: graph_recognition::SelfComplementaryLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_labeled_graphs
   :project: graph_recognition

The enumerator above emits labeled graphs. A second one emits a single
representative per isomorphism class. It is the same complementing-permutation
construction, run on one permutation per valid cycle type instead of on every
permutation: permutations of the same cycle type are conjugate, and conjugating
a complementing permutation maps the graphs it complements to the images of
those graphs, so one representative per cycle type already meets every
isomorphism class that type can produce. From a fixed permutation the graphs
are read off its orbits on vertex pairs — membership alternates along an
orbit and every orbit has even size, so each of the ``r`` orbits admits
exactly two assignments — and flipping all of them only complements the graph,
so ``2^(r-1)`` candidates suffice. Isomorph rejection is a global canonical
form set, because a graph can have complementing permutations of more than one
cycle type.

.. doxygenenum:: graph_recognition::SelfComplementaryUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::SelfComplementaryUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::SelfComplementaryUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_self_complementary_unlabeled_graphs
   :project: graph_recognition


OEIS Count Check
----------------

The non-isomorphic enumeration was verified through ``n = 9`` against
`OEIS A000171 <https://oeis.org/A000171>`_
(``1, 0, 0, 1, 2, 0, 0, 10, 36``), and independently of the enumerator itself
by canonicalizing the labeled enumerator's whole output: its 98280 graphs at
``n = 8`` and its 4123728 at ``n = 9`` collapse to exactly the 10 and 36
classes emitted here. The static test cases stop at ``n = 9``. The cost is
``2^(r-1)`` canonicalizations per cycle type, where ``r`` is the number of
pair orbits, and the exact canonicalization of these highly symmetric graphs
is what bounds the range: ``n <= 9`` runs in well under a second, while
``n = 12`` (``r = 18`` for cycle type (4, 4, 4)) takes hours.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/self_complementary.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_self_complementary(g);

       std::cout << std::boolalpha << result.is_self_complementary << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/self_complementary_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_self_complementary_labeled_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/self_complementary_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_self_complementary_unlabeled_graphs(9);
       std::cout << result.graphs.size() << '\n';  // 36 = A000171(9)
       return 0;
   }


References
----------

* H. Sachs. "Über selbstkomplementäre Graphen."
  *Publicationes Mathematicae Debrecen*, 9:270--288, 1962.

* G. Ringel. "Selbstkomplementäre Graphen."
  *Archiv der Mathematik*, 14(1):354--358, 1963.
  `DOI:10.1007/BF01234967 <https://doi.org/10.1007/BF01234967>`_

* R. C. Read. "On the number of self-complementary graphs and digraphs."
  *Journal of the London Mathematical Society*, s1-38(1):99--104, 1963.
  `DOI:10.1112/jlms/s1-38.1.99 <https://doi.org/10.1112/jlms/s1-38.1.99>`_
