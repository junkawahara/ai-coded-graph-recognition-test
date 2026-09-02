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
     - Directly verifies that every odd cycle of length five or more
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

.. doxygenenum:: graph_recognition::MeynielLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::MeynielLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_meyniel_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/meyniel.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_meyniel(g);

       std::cout << std::boolalpha << result.is_meyniel << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/meyniel_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_meyniel_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* H. Meyniel. "On the perfect graph conjecture."
  *Discrete Mathematics*, 16(4):339--342, 1976.
  `DOI:10.1016/S0012-365X(76)80008-8 <https://doi.org/10.1016/S0012-365X(76)80008-8>`_

* M. Burlet, J. Fonlupt. "Polynomial algorithm to recognize a Meyniel graph."
  *Annals of Discrete Mathematics*, 21:225--252, 1984.
  `DOI:10.1016/S0304-0208(08)72938-4 <https://doi.org/10.1016/S0304-0208(08)72938-4>`_

* F. Roussel, I. Rusu. "An O(n\ :sup:`2`) algorithm to color Meyniel graphs."
  *Discrete Mathematics*, 235(1--3):107--123, 2001.
  `DOI:10.1016/S0012-365X(00)00264-8 <https://doi.org/10.1016/S0012-365X(00)00264-8>`_
