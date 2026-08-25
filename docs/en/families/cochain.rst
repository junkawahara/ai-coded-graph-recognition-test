Co-Chain Graph
==============

Determines whether a graph is a co-chain graph.
A graph whose complement is a chain graph.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CochainAlgorithm``
     - Description
   * - ``COMPLEMENT``
     - Builds the complement and applies chain graph recognition. Complexity: O(n^2).
   * - ``DIRECT`` **(default)**
     - Complement BFS (linked-list technique) for co-bipartite detection,
       then verifies the suffix property. Complexity: O(n^2).

.. doxygenenum:: graph_recognition::CochainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CochainResult
   :project: graph_recognition
   :members:

``CochainResult`` reports the partition into the two cliques (``color``) and
the orders in which the non-neighbourhoods grow by inclusion
(``x_ordering`` / ``y_ordering``) -- the chain structure of the complement.
The ``DIRECT`` variant obtains them without building the complement.

.. doxygenfunction:: graph_recognition::check_cochain
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::CochainEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::CochainEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_cochain_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated non-isomorphic
co-chain graphs was verified, via the complement bijection with chain graphs, to
match `OEIS A005418 <https://oeis.org/A005418>`_: ``1, 2, 3, 6, 10, 20``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "cochain.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_cochain(g);

       std::cout << std::boolalpha << result.is_cochain << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "cochain_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_cochain_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
