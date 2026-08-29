Directed Graph
==============

Recognition and utilities for general directed graphs.

Recognition
-----------

.. doxygenenum:: graph_recognition::DigraphAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::DigraphResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_digraph
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::DigraphLabeledEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_digraphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4``, the number of enumerated labeled directed graphs
was verified to match `OEIS A053763 <https://oeis.org/A053763>`_ (whose
comment identifies ``a(n) = 2^(n^2 - n)`` as the number of simple
digraphs without self-loops on ``n`` labeled nodes): ``1, 4, 64, 4096``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "digraph.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 1}, {2, 3}};
       auto result = check_digraph(3, arcs);

       std::cout << std::boolalpha << result.is_digraph << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "digraph_labeled_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_digraphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* F. Harary, E. M. Palmer. *Graphical Enumeration.*
  Academic Press, 1973.
