Poset (Partially Ordered Set)
=============================

Determines whether a directed graph is the **Hasse diagram** (covering
relation) of a partially ordered set.
The input is interpreted as the covering relation, so an arc ``u -> v``
means ``u`` covers ``v``; the underlying partial order is the reflexive
transitive closure. A valid Hasse diagram is therefore a directed
acyclic graph that is its own transitive reduction. The partial order
itself (with reflexive loops or transitive arcs) must **not** be given
as input.

Recognition
-----------

.. doxygenenum:: graph_recognition::PosetAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PosetResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_poset
   :project: graph_recognition


Enumeration
-----------

.. doxygenstruct:: graph_recognition::PosetEnumeratedGraph
   :project: graph_recognition
   :members:

.. doxygenstruct:: graph_recognition::PosetEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_posets
   :project: graph_recognition

OEIS Count Check
----------------

For ``n = 1, 2, 3, 4, 5``, the number of enumerated labeled posets was
verified to match `OEIS A001035 <https://oeis.org/A001035>`_:
``1, 3, 19, 219, 4231``.


Examples
--------

Recognition example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include <utility>
   #include <vector>
   #include "poset.h"

   int main() {
       using namespace graph_recognition;

       std::vector<std::pair<int, int>> arcs = {{1, 2}, {2, 3}};
       auto result = check_poset(3, arcs);

       std::cout << std::boolalpha << result.is_poset << '\n';
       return 0;
   }

Enumeration example
^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "poset_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_posets(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


References
----------

* R. P. Dilworth. "A decomposition theorem for partially ordered sets."
  *Annals of Mathematics*, 51(1):161--166, 1950.
  `DOI:10.2307/1969503 <https://doi.org/10.2307/1969503>`_

* G. Brinkmann, B. D. McKay. "Posets on up to 16 Points."
  *Order*, 19(2):147--179, 2002.
  `DOI:10.1023/A:1016543307592 <https://doi.org/10.1023/A:1016543307592>`_
