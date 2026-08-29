Graph Recognition Library
=========================

A C++11 header-only library providing recognition algorithms for graph classes.

.. toctree::
   :maxdepth: 2
   :caption: Contents

   getting_started
   usage
   families/index
   api/index
   python


Overview
--------

This library implements recognition algorithms for 75+ graph classes.
Each recognition function accepts an ``enum class`` parameter to select the algorithm,
with a default value so it can be omitted.

.. list-table:: Supported Graph Classes
   :header-rows: 1
   :widths: 25 75

   * - Family
     - Graph Classes
   * - **Chordal**
     - chordal, strongly chordal, proper chordal, split, threshold, weakly chordal, block, ptolemaic, trivially perfect, quasi-threshold, k-tree
   * - **Interval / Arc**
     - interval, proper interval, unit interval, co-interval, circular-arc, proper circular-arc
   * - **Permutation / Comparability**
     - permutation, comparability, co-comparability, bipartite permutation, trapezoid
   * - **Bipartite**
     - bipartite, chordal bipartite, chain, co-chain, convex bipartite, biconvex bipartite
   * - **Planar**
     - planar, outerplanar, cactus, series-parallel, apex, maximal planar, cubic planar, polyhedral, simple quadrangulation, Halin, fullerene
   * - **Perfect / Structural**
     - perfect, cograph, distance-hereditary, AT-free, co-chordal, line graph, circle, Meyniel, parity, even-hole-free, odd-hole-free, cluster, self-complementary
   * - **Forbidden Subgraph**
     - claw-free, diamond-free, triangle-free, bull-free, P5-free, gem-free
   * - **Leaf Power**
     - 3-leaf power, 4-leaf power, 5-leaf power
   * - **Tree / Forest**
     - tree, forest, caterpillar, unicyclic
   * - **Connectivity / Regularity**
     - biconnected, triconnected, Eulerian, k-regular, cubic, strongly regular, snark, Laman
   * - **Directed**
     - tournament, digraph, poset

Most graph classes also support **enumeration** (generating all labeled graphs of a given size)
via reverse search.


Features
--------

* **C++11 compatible** -- no modern compiler required
* **Header-only** -- just ``#include`` and use
* **1-indexed** -- vertex numbers range from 1 to n
* **Namespace** -- all symbols live in ``graph_recognition``
* **Algorithm selection** -- switch algorithms via ``enum class`` parameters


Index
-----

* :ref:`genindex`
