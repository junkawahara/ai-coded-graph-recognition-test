Chain Graph
===========

Determines whether a graph is a chain graph.
A bipartite graph where the neighborhoods within each part are totally ordered by inclusion.

See `ISGCI entry for this class <https://graphclasses.org/classes/gc_442.html>`_ for the definition, inclusions, and complexity of graph problems.

Recognition
-----------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``ChainAlgorithm``
     - Description
   * - ``NEIGHBORHOOD_INCLUSION``
     - Checks all pairs for neighborhood inclusion. Complexity: O(n * m).
   * - ``DEGREE_SORT`` **(default)**
     - Sorts L-side vertices by degree (counting sort) and verifies that each R-side
       vertex's L-side neighbors form a suffix. Complexity: O(n + m).

.. doxygenenum:: graph_recognition::ChainAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainResult
   :project: graph_recognition
   :members:

``ChainResult`` also reports the nested orders themselves: ``color`` is the
bipartition, and ``x_ordering`` / ``y_ordering`` list each side so that the
neighbourhoods grow by inclusion. Sorting a side by degree produces that
order whenever the graph really is a chain graph.

.. doxygenfunction:: graph_recognition::check_chain
   :project: graph_recognition


Enumeration
-----------

.. doxygenenum:: graph_recognition::ChainEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::ChainEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::ChainEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_chain_graphs
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5, 6``, the number of enumerated non-isomorphic
chain graphs was verified to match `OEIS A005418
<https://oeis.org/A005418>`_ (whose comment identifies the sequence as
the number of ``n``-vertex difference graphs, i.e. bipartite 2K_2-free
graphs, after Peled & Sun): ``1, 2, 3, 6, 10, 20``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "chain.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_chain(g);

       std::cout << std::boolalpha << result.is_chain << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "chain_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_chain_graphs(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* M. Yannakakis. "The complexity of the partial order dimension problem."
  *SIAM Journal on Algebraic and Discrete Methods*, 3(3):351--358, 1982.
  `DOI:10.1137/0603036 <https://doi.org/10.1137/0603036>`_
