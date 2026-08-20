サークルグラフ
============================

**サークルグラフ** は、円の弦の集合の交差グラフである。2 頂点が隣接するのは、
対応する弦が交差するとき、かつそのときに限る。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_132.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircleAlgorithm``
     - 説明
   * - ``NAJI_SYSTEM`` **(既定)**
     - Naji の特徴付けによる方法: グラフがサークルグラフであることと、
       GF(2) 上のある連立一次方程式 (相異なる頂点の順序対ごとに 1 変数)
       が可解であることは同値。可解性はビットセットによるガウス消去で
       多項式時間で判定する。判定のみで、弦図は生成しない。
       実装はまず双子頂点 (twin) のクラスを縮約して入力を縮小し、密な
       GF(2) 基底のメモリ使用量に上限を設けている (超過時は
       ``std::runtime_error`` を送出)。
       (Naji 1985; Gasse, *Discrete Math.* 173, 1997; Geelen–Lee, *J. Graph Theory* 93, 2020)
   * - ``DOW_BACKTRACKING``
     - DOW (double occurrence word) による弦図バックトラッキング。弦の端点を
       円周上に配置して、入力グラフと整合する弦図の構築を試みる。
       YES の場合は明示的な DOW 証明書を返すが、最悪の場合は指数時間かかる
       (実用上は n = 9 程度まで; NO の判定側が高コスト)。
       探索にはステップ数の予算が設けられており、使い切った場合は無限に
       走り続ける代わりに ``std::runtime_error`` を送出する。

.. doxygenenum:: graph_recognition::CircleAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_circle
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::CircleEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircleEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circle_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "circle.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_circle(g);

       std::cout << std::boolalpha << result.is_circle << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circle_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circle_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
----------------

* S. Even, A. Itai. "Queues, stacks and graphs."
  In Z. Kohavi, A. Paz (eds.), *Theory of Machines and Computations*, Academic Press,
  pp. 71--86, 1971.

* J. P. Spinrad. "Recognition of circle graphs."
  *Journal of Algorithms*, 16(2):264--282, 1994.
  `DOI:10.1006/jagm.1994.1012 <https://doi.org/10.1006/jagm.1994.1012>`_
