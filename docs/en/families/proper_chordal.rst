Proper Chordal Graph
====================

A graph is **proper chordal** if it is a chordal graph admitting an
indifference tree-layout (Paul & Protopapas 2024). Note that "subtree
intersection representation with no subtree contained in another" does NOT
define this class: every chordal graph has such a representation (containment
can always be broken by attaching a private new leaf per subtree).

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_1363.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. doxygenstruct:: graph_recognition::ProperChordalResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_proper_chordal
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ProperChordalEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ProperChordalEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_proper_chordal_graphs_reverse_search
   :project: graph_recognition


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "proper_chordal.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_proper_chordal(g);

       std::cout << std::boolalpha << result.is_proper_chordal << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "proper_chordal_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_proper_chordal_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* C. Paul, E. Protopapas. "Proper chordal graphs."
  *Proceedings of STACS 2024*, LIPIcs 289, 53:1--53:17, 2024.
  `DOI:10.4230/LIPIcs.STACS.2024.53 <https://doi.org/10.4230/LIPIcs.STACS.2024.53>`_
