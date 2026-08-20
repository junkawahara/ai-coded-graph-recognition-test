円弧グラフ
==============

グラフが円弧グラフかどうかを判定する。
各頂点は円周上の弧に対応し、弧が重なる頂点間に辺を持つ。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_133.html>`_ を参照。

認識
------------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``CircularArcAlgorithm``
     - 説明
   * - ``MCCONNELL`` **(既定)**
     - 極大クリークを列挙し、円環状のクリーク順序 (circular clique
       ordering) を構築する。Helly な円弧グラフの入力に対しては多項式時間
       だが、非 Helly のケースではクリーク順序上のバックトラッキングに
       フォールバックし、最悪ケースは指数時間。
   * - ``BACKTRACKING``
     - 円周上の端点順序をバックトラッキングで探索し、2-SAT による
       枝刈りを行う。指数時間 (小規模グラフ向け)。

.. doxygenenum:: graph_recognition::CircularArcAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircularArcResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_circular_arc
   :project: graph_recognition


列挙
------------

.. doxygenenum:: graph_recognition::CircularArcEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::CircularArcEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_circular_arc_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "circular_arc.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_circular_arc(g);

       std::cout << std::boolalpha << result.is_circular_arc << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "circular_arc_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_circular_arc_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
--------------

* A. Tucker. "An efficient test for circular-arc graphs."
  *SIAM Journal on Computing*, 9(1):1--24, 1980.
  `DOI:10.1137/0209001 <https://doi.org/10.1137/0209001>`_

* R. M. McConnell. "Linear-time recognition of circular-arc graphs."
  *Algorithmica*, 37(2):93--147, 2003.
  `DOI:10.1007/s00453-003-1032-7 <https://doi.org/10.1007/s00453-003-1032-7>`_
