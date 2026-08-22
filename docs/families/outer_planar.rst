外平面グラフ
========================

全ての頂点が外面上に位置するように平面に埋め込めるグラフを
**外平面グラフ** という。これは K\ :sub:`4` マイナーも
K\ :sub:`2,3` マイナーも含まないことと同値である。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_110.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``OuterPlanarAlgorithm``
     - 説明
   * - ``AUGMENTED_PLANARITY`` **(既定)**
     - 拡張グラフ G + K\ :sub:`1` (他の全頂点に隣接する新頂点を追加したグラフ) の平面性を検査する。G が外平面的であることと G + K\ :sub:`1` が平面的であることは同値。
   * - ``MINOR_CHECK``
     - 辺数上界 (2n - 3) の事前検査の後、K\ :sub:`4` と K\ :sub:`2,3` のマイナーをバックトラッキングで探索する。

.. doxygenenum:: graph_recognition::OuterPlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_outer_planar
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::OuterPlanarEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::OuterPlanarEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_outer_planar_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き外平面グラフの個数が
`OEIS A098000 <https://oeis.org/A098000>`_ と一致することを検証した:
``1, 2, 8, 63, 893``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "outer_planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_outer_planar(g);

       std::cout << std::boolalpha << result.is_outer_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "outer_planar_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_outer_planar_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* G. Chartrand, F. Harary. "Planar permutation graphs."
  *Annales de l'Institut Henri Poincaré B*, 3(4):433--438, 1967.

* S. L. Mitchell. "Linear algorithms to recognize outerplanar and maximal outerplanar graphs."
  *Information Processing Letters*, 9(5):229--232, 1979.
  `DOI:10.1016/0020-0190(79)90075-9 <https://doi.org/10.1016/0020-0190(79)90075-9>`_
