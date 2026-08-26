Circle Graph
============

A **circle graph** is the intersection graph of a set of chords of a
circle.  Two vertices are adjacent if and only if their corresponding chords
intersect.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_132.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircleAlgorithm``
     - Description
   * - ``NAJI_SYSTEM`` **(default)**
     - Naji's characterization: the graph is a circle graph iff a certain system of
       linear equations over GF(2) (one variable per ordered pair of distinct vertices)
       is solvable. Solvability is decided by bitset Gaussian elimination in polynomial
       time. Decision only; no chord diagram is produced.
       The implementation first contracts twin classes to shrink the input, and
       enforces a memory limit on its dense GF(2) basis (throws
       ``std::runtime_error`` when exceeded).
       (Naji 1985; Gasse, *Discrete Math.* 173, 1997; Geelen–Lee, *J. Graph Theory* 93, 2020)
   * - ``DOW_BACKTRACKING``
     - DOW (double occurrence word) chord-diagram backtracking. Attempts to construct a chord diagram
       consistent with the input graph by placing chord endpoints on a circle.
       Produces an explicit DOW certificate on YES, but takes exponential time in the
       worst case (practical up to roughly n = 9; NO answers are the expensive side).
       The search runs under a step budget and throws ``std::runtime_error``
       when it is exhausted instead of running forever.

.. doxygenenum:: graph_recognition::CircleAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_circle
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CircleLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circle_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "circle.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_circle(g);

       std::cout << std::boolalpha << result.is_circle << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circle_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circle_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* W. Naji. "Reconnaissance des graphes de cordes."
  *Discrete Mathematics*, 54(3):329--337, 1985.
  `DOI:10.1016/0012-365X(85)90117-7 <https://doi.org/10.1016/0012-365X(85)90117-7>`_

* E. Gasse. "A proof of a circle graph characterization."
  *Discrete Mathematics*, 173(1--3):277--283, 1997.
  `DOI:10.1016/S0012-365X(97)00068-X <https://doi.org/10.1016/S0012-365X(97)00068-X>`_

* J. Geelen, E. Lee. "Naji's characterization of circle graphs."
  *Journal of Graph Theory*, 93(1):21--33, 2020.
  `DOI:10.1002/jgt.22466 <https://doi.org/10.1002/jgt.22466>`_

* S. Even, A. Itai. "Queues, stacks and graphs."
  In Z. Kohavi, A. Paz (eds.), *Theory of Machines and Computations*, Academic Press,
  pp. 71--86, 1971.

* J. P. Spinrad. "Recognition of circle graphs."
  *Journal of Algorithms*, 16(2):264--282, 1994.
  `DOI:10.1006/jagm.1994.1012 <https://doi.org/10.1006/jagm.1994.1012>`_
