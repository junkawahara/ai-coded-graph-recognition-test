Chordal Graph
=============

A graph is **chordal** (triangulated) if it contains no induced cycle of length
four or more. Equivalently, the graph admits a perfect elimination ordering
(PEO).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_32.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChordalAlgorithm``
     - Description
   * - ``MCS_PEO``
     - Priority-queue Maximum Cardinality Search, O(n + m log n)
   * - ``BUCKET_MCS_PEO`` **(default)**
     - Bucket-sort Maximum Cardinality Search, O(n + m)
   * - ``LEXBFS_PEO``
     - LexBFS by Rose-Tarjan-Lueker 1976, O(n + m)

.. doxygenenum:: graph_recognition::ChordalAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::EnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chordal_graphs_reverse_search
   :project: graph_recognition


OEIS Count Check
----------------

For ``n = 2, 3, 4, 5, 6``, the number of enumerated labeled chordal graphs
was verified to match `OEIS A058862 <https://oeis.org/A058862>`_:
``2, 8, 61, 822, 18154``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chordal(g);

       std::cout << std::boolalpha << result.is_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* D. R. Fulkerson, O. A. Gross. "Incidence matrices and interval graphs."
  *Pacific Journal of Mathematics*, 15(3):835--855, 1965.
  `DOI:10.2140/pjm.1965.15.835 <https://doi.org/10.2140/pjm.1965.15.835>`_

* D. J. Rose, R. E. Tarjan, G. S. Lueker. "Algorithmic aspects of vertex elimination on graphs."
  *SIAM Journal on Computing*, 5(2):266--283, 1976.
  `DOI:10.1137/0205021 <https://doi.org/10.1137/0205021>`_

* R. E. Tarjan, M. Yannakakis. "Simple linear-time algorithms to test chordality of graphs, test acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs."
  *SIAM Journal on Computing*, 13(3):566--579, 1984.
  `DOI:10.1137/0213035 <https://doi.org/10.1137/0213035>`_
