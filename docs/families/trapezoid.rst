台形グラフ
====================

グラフが台形グラフであるかを判定する。
頂点は 2 本の水平線の間に置かれた台形に対応し、
重なり合う台形同士が辺で結ばれる。
対応する半順序のインターバル次元が高々 2 である
co-comparability グラフであることと同値。

定義・クラス包含関係・各種問題の計算量については `ISGCI のクラスページ <https://graphclasses.org/classes/gc_59.html>`_ を参照。

認識
--------

.. list-table::
   :header-rows: 1
   :widths: 30 70

   * - ``TrapezoidAlgorithm``
     - 説明
   * - ``CHAIN_COVER`` **(既定)**
     - Cogis (1982) の特徴付けに基づく。
       (1) 補グラフの推移的向き付けにより co-comparability 性を判定する。
       (2) 対応する半順序 P の二部グラフ B(P)
       (辺 (x,y) ⟺ NOT x <_P y) を構築し、非両立グラフ I(B)
       (2+2 パターンから得られる辺) の二部性を BFS で検査する。
       自明な 2K_2 (相異なる元が 4 個未満のもの) は除外する。

.. doxygenenum:: graph_recognition::TrapezoidAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TrapezoidResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::check_trapezoid
   :project: graph_recognition


列挙
--------

.. doxygenenum:: graph_recognition::TrapezoidEnumAlgorithm
   :project: graph_recognition

.. doxygenstruct:: graph_recognition::TrapezoidEnumerationResult
   :project: graph_recognition
   :members:

.. doxygenfunction:: graph_recognition::enumerate_trapezoid_graphs_reverse_search
   :project: graph_recognition


使用例
------------

認識の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "graph.h"
   #include "trapezoid.h"

   int main() {
       using namespace graph_recognition;

       Graph g(4, {{1, 2}, {2, 3}, {3, 4}});
       auto result = check_trapezoid(g);

       std::cout << std::boolalpha << result.is_trapezoid << '\n';
       return 0;
   }

列挙の例
^^^^^^^^^^^^

.. code-block:: cpp

   #include <iostream>
   #include "trapezoid_enum.h"

   int main() {
       using namespace graph_recognition;

       auto result = enumerate_trapezoid_graphs_reverse_search(4);
       std::cout << result.graphs.size() << '\n';
       return 0;
   }


参考文献
------------

* I. Dagan, M. C. Golumbic, R. Y. Pinter. "Trapezoid graphs and their coloring."
  *Discrete Applied Mathematics*, 21(1):35--46, 1988.
  `DOI:10.1016/0166-218X(88)90032-7 <https://doi.org/10.1016/0166-218X(88)90032-7>`_

* O. Cogis. "On the Ferrers dimension of a digraph."
  *Discrete Mathematics*, 38(1):47--52, 1982.
  `DOI:10.1016/0012-365X(82)90167-4 <https://doi.org/10.1016/0012-365X(82)90167-4>`_

* T.-H. Ma, J. P. Spinrad. "On the 2-chain subgraph cover and related problems."
  *Journal of Algorithms*, 17(2):251--268, 1994.
  `DOI:10.1006/jagm.1994.1034 <https://doi.org/10.1006/jagm.1994.1034>`_
