Co-Chordal Graph
================

A graph is **co-chordal** if its complement is a chordal graph.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_145.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CoChordalAlgorithm``
     - Description
   * - ``COMPLEMENT`` **(default)**
     - Builds the complement graph and applies chordal recognition.

.. doxygenenum:: graph_recognition::CoChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_co_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::CoChordalLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CoChordalLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_co_chordal_labeled_graphs_reverse_search
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled co-chordal
graphs was verified, via the complement bijection with chordal graphs, to match
`OEIS A058862 <https://oeis.org/A058862>`_: ``2, 8, 61, 822, 18154``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "co_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_co_chordal(g);

       std::cout << std::boolalpha << result.is_co_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "co_chordal_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_co_chordal_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
