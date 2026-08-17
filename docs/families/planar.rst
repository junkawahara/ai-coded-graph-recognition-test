平面グラフ
====================

辺の交差なしに平面に描画できるグラフを **平面グラフ** という。
Wagner の定理により、グラフが平面的であることと、
K\ :sub:`5` マイナーも K\ :sub:`3,3` マイナーも含まないことは同値である。
(Kuratowski の定理は、マイナーの代わりに細分を用いた同値な特徴付けを与える。)

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_43.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``PlanarAlgorithm``
     - 説明
   * - ``LEFT_RIGHT`` **(default)**
     - 辺数上界 (3n - 6) の事前検査の後、de Fraysseix, Ossona de Mendez,
       Rosenstiehl の left-right 平面性判定条件 (競合ペアのスタックを用いた
       2 回の DFS) を適用する。計算量は O(n + m)。
   * - ``MINOR_CHECK``
     - 辺数上界 (3n - 6) の事前検査の後、K\ :sub:`5` と K\ :sub:`3,3` のマイナーをバックトラッキングで探索する。
       正確だが最悪の場合は指数時間であり、小さなグラフのクロスチェック用として残している。

.. doxygenenum:: graph_recognition::PlanarAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_planar
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::PlanarEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::PlanarEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_planar_graphs_reverse_search
   :project: graph_recognition

OEIS カウント検証
--------------------------------

``n = 1, 2, 3, 4, 5`` について、列挙されたラベル付き平面グラフの個数が
`OEIS A066537 <https://oeis.org/A066537>`_ と一致することを検証した:
``1, 2, 8, 64, 1023``。


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "planar.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_planar(g);

       std::cout << std::boolalpha << result.is_planar << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "planar_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_planar_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* K. Kuratowski. "Sur le problème des courbes gauches en topologie."
  *Fundamenta Mathematicae*, 15(1):271--283, 1930.
  `DOI:10.4064/fm-15-1-271-283 <https://doi.org/10.4064/fm-15-1-271-283>`_

* K. Wagner. "Über eine Eigenschaft der ebenen Komplexe."
  *Mathematische Annalen*, 114(1):570--590, 1937.
  `DOI:10.1007/BF01594196 <https://doi.org/10.1007/BF01594196>`_

* J. Hopcroft, R. Tarjan. "Efficient planarity testing."
  *Journal of the ACM*, 21(4):549--568, 1974.
  `DOI:10.1145/321850.321852 <https://doi.org/10.1145/321850.321852>`_
