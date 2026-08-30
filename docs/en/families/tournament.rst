Tournament
==========

Determines whether a directed graph is a tournament.
A complete directed graph: for every pair of vertices, exactly one directed arc exists.

Recognition
-----------

.. doxygenenum:: graph_recognition::TournamentAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TournamentResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_tournament
   :project: graph_recognition

.. doxygenfunction:: graph_recognition::read_directed
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::TournamentLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournaments
   :project: graph_recognition

The enumerator above emits labeled tournaments. A second one emits a
single representative per isomorphism class, by McKay's canonical
construction path method — the same scheme as nauty's dedicated
tournament generator gentourng. Tournaments are grown one vertex at a
time (deleting any vertex of a tournament again yields a tournament, so
every class is reachable), each child adds one of the ``2^k``
orientation patterns against the existing vertices, and no recognition
filter is needed because every intermediate digraph is a tournament by
construction. Isomorph rejection reuses the shared undirected
canonicalization on the out-adjacency bitmasks, which is sound only
because the underlying graph is complete: the canonical form packs each
row against earlier positions only, and in a tournament an unset bit is
exactly the reversed arc, so the form determines every pair's
orientation. Practical to about ``n = 9`` (191536 classes). Every
tournament is weakly connected, so there is no ``connected_only`` flag.

.. doxygenenum:: graph_recognition::TournamentUnlabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TournamentUnlabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::TournamentUnlabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_tournament_unlabeled_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated labeled tournaments
was verified to match `OEIS A006125 <https://oeis.org/A006125>`_ (whose
comment identifies ``a(n) = 2^(n(n-1)/2)`` as the number of outcomes of
labeled n-team round-robin tournaments): ``1, 2, 8, 64, 1024, 32768``.

The non-isomorphic enumeration reproduces
`OEIS A000568 <https://oeis.org/A000568>`_ (tournaments on n unlabeled
nodes): ``1, 1, 2, 4, 12, 56, 456, 6880`` for ``n = 1, ..., 8`` in the
static test cases, plus ``191536`` at ``n = 9`` (about 9 s) checked by
hand. For ``n <= 6`` the canonicalized output of the labeled enumerator
yields exactly the same isomorphism classes.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "tournament.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {1, 3}, {2, 3}};
       auto result = check_tournament(3, arcs);

       std::cout << std::boolalpha << result.is_tournament << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tournament_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournaments(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }

Non-isomorphic enumeration example
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "tournament_unlabeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_tournament_unlabeled_graphs(5);
       std::cout << result.graphs.size() << '\n';  // 12
       return 0;
   }


References
----------

* J. W. Moon. *Topics on Tournaments.*
  Holt, Rinehart and Winston, New York, 1968.

* B. D. McKay. "Isomorph-free exhaustive generation."
  *Journal of Algorithms*, 26(2):306--324, 1998.
  `DOI:10.1006/jagm.1997.0898 <https://doi.org/10.1006/jagm.1997.0898>`_
