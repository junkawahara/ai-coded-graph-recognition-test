Apex Graph
==========

A graph is an **apex graph** if removing a single vertex makes it planar.
The algorithm tests planarity of G - v for each vertex v.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1181.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenenum:: graph_recognition::ApexAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApexResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_apex
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ApexLabeledEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ApexLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_apex_labeled_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "util/graph.h"
   #include "recognizers/apex.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_apex(g);

       std::cout << std::boolalpha << result.is_apex << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "enumerators/apex_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_apex_labeled_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* N. Robertson, P. D. Seymour. "Graph minors. XIII. The disjoint paths problem."
  *Journal of Combinatorial Theory, Series B*, 63(1):65--110, 1995.
  `DOI:10.1006/jctb.1995.1006 <https://doi.org/10.1006/jctb.1995.1006>`_

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_
