Connectivity / Regularity Family
================================

Graphs defined by connectivity conditions or degree regularity constraints.


biconnected.h -- Biconnected Graph
------------------------------------

Determines whether a graph is biconnected (2-connected).
A connected graph with no cut vertex.

.. doxygenfile:: biconnected.h
   :project: graph_recognition


biconnected_enum.h -- Biconnected Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: biconnected_enum.h
   :project: graph_recognition


triconnected.h -- Triconnected Graph
--------------------------------------

Determines whether a graph is triconnected (3-connected).
A graph that remains connected after removing any two vertices.

.. doxygenfile:: triconnected.h
   :project: graph_recognition


eulerian.h -- Eulerian Graph
------------------------------

Determines whether a graph is Eulerian.
A connected graph where every vertex has even degree.

.. doxygenfile:: eulerian.h
   :project: graph_recognition


eulerian_enum.h -- Eulerian Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: eulerian_enum.h
   :project: graph_recognition


kregular.h -- K-Regular Graph
-------------------------------

Determines whether a graph is k-regular.
A graph where every vertex has the same degree k.

.. doxygenfile:: kregular.h
   :project: graph_recognition


kregular_enum.h -- K-Regular Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: kregular_enum.h
   :project: graph_recognition


cubic.h -- Cubic Graph
------------------------

Determines whether a graph is cubic (3-regular).
A graph where every vertex has degree exactly 3.

.. doxygenfile:: cubic.h
   :project: graph_recognition


cubic_enum.h -- Cubic Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: cubic_enum.h
   :project: graph_recognition


strongly_regular.h -- Strongly Regular Graph
----------------------------------------------

Determines whether a graph is strongly regular with parameters (n, k, lambda, mu).
A k-regular graph where every pair of adjacent vertices has exactly lambda common neighbors
and every pair of non-adjacent vertices has exactly mu common neighbors.

.. doxygenfile:: strongly_regular.h
   :project: graph_recognition


strongly_regular_enum.h -- Strongly Regular Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: strongly_regular_enum.h
   :project: graph_recognition


snark.h -- Snark
-----------------

Determines whether a graph is a snark.
A bridgeless cubic graph that is not 3-edge-colorable.

.. doxygenfile:: snark.h
   :project: graph_recognition


snark_enum.h -- Snark Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: snark_enum.h
   :project: graph_recognition


laman.h -- Laman Graph
------------------------

Determines whether a graph is a Laman graph.
A graph satisfying the Laman condition: m = 2n - 3 and every subgraph on k vertices
has at most 2k - 3 edges. Laman graphs characterize minimally rigid frameworks in the plane.

.. doxygenfile:: laman.h
   :project: graph_recognition


laman_enum.h -- Laman Graph Enumeration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. doxygenfile:: laman_enum.h
   :project: graph_recognition
