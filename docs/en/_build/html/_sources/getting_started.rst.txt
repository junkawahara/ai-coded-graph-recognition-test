Getting Started
===============

Installation
------------

The library is header-only. Clone or copy the ``include/`` directory into your project:

.. code-block:: bash

   git clone https://github.com/junkawahara/ai-coded-graph-recognition-test.git
   # Add -Ipath/to/include to your compiler flags

No build step is required for the library itself.
The repository's ``Makefile`` builds CLI executables and tests:

.. code-block:: bash

   make          # Build all CLI targets
   make clean    # Remove binaries


C++ Quick Start
---------------

.. code-block:: cpp

   #include "util/graph.h"
   #include "recognizers/interval.h"
   #include <iostream>

   int main() {
       using namespace graph_recognition;
       Graph g = Graph::read(std::cin);
       IntervalResult res = check_interval(g);
       if (res.is_interval) {
           std::cout << "YES" << std::endl;
           for (int v = 1; v <= g.n; ++v) {
               std::cout << v << ": ["
                         << res.intervals[v].first << ", "
                         << res.intervals[v].second << "]"
                         << std::endl;
           }
       } else {
           std::cout << "NO" << std::endl;
       }
       return 0;
   }


Python Quick Start
------------------

.. code-block:: bash

   pip install "graph-recognition @ git+https://github.com/junkawahara/ai-coded-graph-recognition-test.git#subdirectory=python"

.. code-block:: python

   from graph_recognition import is_interval, is_chordal

   is_interval(4, [(1, 2), (2, 3), (3, 4)])    # True
   is_interval(4, [(1, 2), (2, 3), (3, 4), (4, 1)])  # False

   is_chordal(4, [(1, 2), (2, 3), (3, 4)])      # True


Next Steps
----------

* :doc:`usage` -- input/output format, algorithm selection, testing
* :doc:`families/index` -- per-family API reference (recognition + enumeration)
* :doc:`python` -- full Python API reference
