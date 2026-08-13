Usage Reference
===============

Build
-----

.. code-block:: bash

   make          # Build all targets
   make clean    # Remove binaries

The compiler defaults to ``g++`` with C++11. Override with ``CXX`` and ``CXXFLAGS``.


Input / Output Format
---------------------

**Input** (stdin):

.. code-block:: text

   n m
   u1 v1
   u2 v2
   ...

* ``n``: number of vertices, ``m``: number of edges
* Vertices are 1-indexed

**Output** (stdout): ``YES`` / ``NO`` followed by class-specific information.


Algorithm Selection
-------------------

Each recognition function accepts an ``enum class`` parameter to choose the algorithm.
A default argument is provided, so the parameter can be omitted.

.. code-block:: cpp

   #include "graph.h"
   #include "interval.h"

   using namespace graph_recognition;
   Graph g = Graph::read(std::cin);

   // Use the default algorithm (AT_FREE)
   IntervalResult r1 = check_interval(g);

   // Explicitly select an algorithm
   IntervalResult r2 = check_interval(g, IntervalAlgorithm::BACKTRACKING);
   IntervalResult r3 = check_interval(g, IntervalAlgorithm::AT_FREE);

More examples:

.. code-block:: cpp

   // Permutation graph
   check_permutation(g, PermutationAlgorithm::BACKTRACKING);
   check_permutation(g, PermutationAlgorithm::CLASS_BASED);  // default

   // Chordal bipartite graph
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::CYCLE_CHECK);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::BISIMPLICIAL);
   check_chordal_bipartite(g, ChordalBipartiteAlgorithm::FAST_BISIMPLICIAL);  // default

   // Distance-hereditary graph
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::HASH_TWINS);  // default
   check_distance_hereditary(g, DistanceHereditaryAlgorithm::SORTED_TWINS);


CLI Executables
---------------

Each graph class has a corresponding CLI executable in ``src/<type>_main.cpp``.
After building with ``make``, run:

.. code-block:: bash

   printf "4 3\n1 2\n2 3\n3 4\n" | ./interval
   # Output: YES followed by the interval model

Enumeration executables accept a vertex count:

.. code-block:: bash

   echo 5 | ./interval_enum
   # Outputs all labeled interval graphs on 5 vertices


Testing
-------

**Primary test suite** (Google Test):

.. code-block:: bash

   make test                                # default filter, ~100 s
   make test-quick                          # exclude only the property tests
   ./gtest_all --gtest_filter='Interval*'   # run a subset

Legacy per-class checkers, differential testers, and fuzzers live in
``tests/legacy/`` (``check_<type>.py``, ``compare.py``, ``fuzz.sh``);
some of their internal paths still assume the pre-migration layout, so
prefer the gtest suite above.
